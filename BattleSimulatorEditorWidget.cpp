#include "BattleSimulatorEditorWidget.h"

#include "EditorAssetLibrary.h"
#include "EditorUtilityTask.h"
#include "DataTable/Item/CommonTableEnumType.h"
#include "ToolTable/UPToolTableInfo.h"
#include "JsonObjectConverter.h"
#include "GameDelegates.h"
#include "LevelEditor.h"
#include "WidgetBlueprint.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Dom/JsonObject.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/FileHelper.h"
#include "Client/UPGameHelper.h"
#include "Client/GameMode/UPBattleTestGameMode.h"
#include "DataTable/UPDataAssetTableManager.h"
#include "GameFramework/WorldSettings.h"
#include "Slate\UPEditorSkillListPanelEditorWidget.h"
#include "EditorUtilitySubsystem.h"

#include "ToolTable/UPToolTableManager.h"


#define LOCTEXT_NAMESPACE "BattleSimulatorEditorWidget"

class IDesktopPlatform;

template <typename T>
FString EnumToString(const T& EnumValue)
{
   const UEnum* EnumClass = StaticEnum<T>();
   if (!EnumClass)
   {
      return TEXT("");
   }

   const FString& PathName = EnumClass->GetPathName();
   int32 LastIndex = 0;
   PathName.FindLastChar('/', LastIndex);
   const FString EnumPath = PathName.RightChop(LastIndex + 1);
   return UEnum::GetValueAsString(*EnumPath, EnumValue);
}


UBattleSimulatorEditorWidget::UBattleSimulatorEditorWidget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
   bAutoRunDefaultAction = true;
   TabDisplayName = FText::FromString(TEXT("Editor Tool Panel"));
}

UBattleSimulatorEditorWidget::~UBattleSimulatorEditorWidget()
{
   if(SkillEditorWidget.IsValid())
   {
      SkillEditorWidget.Reset();
   }
}

void UBattleSimulatorEditorWidget::NativeConstruct()
{
   Super::NativeConstruct();
   FGameDelegates::Get().GetEndPlayMapDelegate().RemoveAll(this);
}

bool UBattleSimulatorEditorWidget::Initialize()
{
    const bool SuperInitialized = Super::Initialize();
   EObjectFlags flags = GetFlags();
   if (SuperInitialized && HasAnyFlags(RF_Transactional))
   {
      GetAllMapNames();
      ReadTable();
      ReadValidCheckInfo();
      LoadFromIni();
      ExecuteDefaultAction();
   }

   return SuperInitialized;
}

void UBattleSimulatorEditorWidget::ReleaseSlateResources(bool bReleaseChildren)
{
   Super::ReleaseSlateResources(bReleaseChildren);
   if(SkillEditorWidget.IsValid())
   {
      SkillEditorWidget.Reset();
   }

   FText TabName = GetTabDisplayName();
   FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName(*TabName.ToString()));
}

void UBattleSimulatorEditorWidget::Run_Implementation()
{
   // 슬레이트 창 띄우기
}


bool UBattleSimulatorEditorWidget::ReadTable()
{
   {
      TableCharacterDatas.Empty();
      TableCharacterDatas =  FUPToolTableManager::GetInstance()->GetToolDataPtrByType<FToolCharacterData>(EToolTableType::CharacterTable);
      OnReadCharacterTable(TableCharacterDatas);
    }
   
     {
       const TArray<FToolZoneInfo> RetData = FUPToolTableManager::GetInstance()->GetToolDataPtrByType<FToolZoneInfo>(EToolTableType::ZoneInfo);
      OnReadZoneInfoTable(RetData);
     }
   
   {
      const TArray<FToolZoneSpawnInfo> RetData = FUPToolTableManager::GetInstance()->GetToolDataPtrByType<FToolZoneSpawnInfo>(EToolTableType::ZoneSpawnInfo);
      OnReadZoneSpawnTable(RetData);
   }
   return true;   
}

bool UBattleSimulatorEditorWidget::ReloadTable()
{
   FUPToolTableManager::GetInstance()->ReloadDataTable();
   return ReadTable();
}


void UBattleSimulatorEditorWidget::SaveDataToIni(TArray<FToolCharacterData> NewPlayerCharacterDataList,
                                                 TArray<FToolCharacterData> NewEnemyCharacterDataList,
                                                 const FString& InLevelID, const FString& InZoneAssetName,
                                                 const TArray<FString>& InCommandList)
{
   UToolCharacterToolSettingData* PlayerCharacterSettings = UToolCharacterToolSettingData::GetCharacterToolSettingData();
   
   FString ContentDir = FPaths::ProjectConfigDir();

   FString ToolDataInitPath = TEXT("ToolData.ini");
   FString EditorCharacterSettingPath;
   
   //ToolData.ini 저장.
   FString IniPath = FPaths::Combine(ContentDir, ToolDataInitPath);
   PlayerCharacterSettings->BattleCharacterData.PlayerCharacterDatas.Empty();
   PlayerCharacterSettings->BattleCharacterData.PlayerCharacterDatas.Reserve(5);
      
   for(int i=0; i<NewPlayerCharacterDataList.Num(); ++i) 
   {
      PlayerCharacterSettings->BattleCharacterData.PlayerCharacterDatas.Emplace(NewPlayerCharacterDataList[i]);
      //Enum값이 0일 경우, ini로 저장이 되지 않아서 넣은 코드
      PlayerCharacterSettings->BattleCharacterData.PlayerCharacterDatas[i].SpawnPosition = static_cast<ECreatureTargetPoint>(NewPlayerCharacterDataList[i].SpawnPosition);
      //~Enum값이 0일 경우, ini로 저장이 되지 않아서 넣은 코드
   }
   PlayerCharacterSettings->BattleCharacterData.EnemyCharacterDatas.Empty();        
   PlayerCharacterSettings->BattleCharacterData.EnemyCharacterDatas.Reserve(5);     

   for(int i=0; i<NewEnemyCharacterDataList.Num(); ++i) 
   {
      PlayerCharacterSettings->BattleCharacterData.EnemyCharacterDatas.Emplace(NewEnemyCharacterDataList[i]);
      //Enum값이 0일 경우, ini로 저장이 되지 않아서 넣은 코드
      PlayerCharacterSettings->BattleCharacterData.EnemyCharacterDatas[i].SpawnPosition = static_cast<ECreatureTargetPoint>(NewEnemyCharacterDataList[i].SpawnPosition);
      //~Enum값이 0일 경우, ini로 저장이 되지 않아서 넣은 코드
   }
   PlayerCharacterSettings->ZoneTableID = InLevelID;
   PlayerCharacterSettings->ZoneAssetName = InZoneAssetName;
   
   PlayerCharacterSettings->CommandList.Empty();
   PlayerCharacterSettings->CommandList.Append(InCommandList);
      
   PlayerCharacterSettings->SaveConfigs();
}

void UBattleSimulatorEditorWidget::LoadFromIni()
{
   UToolCharacterToolSettingData* PlayerCharacterSettings = UToolCharacterToolSettingData::GetCharacterToolSettingData();
   PlayerCharacterSettings->LoadConfig();

   if(PlayerCharacterSettings == nullptr)
   {
      return;
   }
   OnLoadIniDataCompleted(PlayerCharacterSettings);
   TArray<FToolZoneInfo> ZoneInfos = FUPToolTableManager::GetInstance()->GetToolDataPtrByType<FToolZoneInfo>(EToolTableType::ZoneInfo);
   const FString LevelName = PlayerCharacterSettings->ZoneTableID;
   const int32 FindIndex = ZoneInfos.IndexOfByPredicate([LevelName](const FToolZoneInfo& Element)
   {
      return Element.ZoneName == LevelName;
   });
   if (FindIndex == INDEX_NONE)
   {
      return;
   }

   SetZoneAssetName(ZoneInfos[FindIndex].ZoneAssetName);
}

bool UBattleSimulatorEditorWidget::SaveToJson(
   const TArray<FToolCharacterData>& NewPlayerCharacterDataList, const TArray<FToolCharacterData>& NewEnemyCharacterDataList)
{
   const FString PPConvertPath = FPaths::GetRootContentDir();
   const FString PackageNameSuggestion = FString(TEXT("/Game/")) + PPConvertPath + FString(TEXT("/"));

   FString PackagePath;
   FPackageName::TryConvertFilenameToLongPackageName(PackageNameSuggestion, PackagePath);

   IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
   if (DesktopPlatform == nullptr)
   {
      return false;
   }
   
   TArray<FString> OpenFilenames;
   const bool bOpened = DesktopPlatform->SaveFileDialog(
      FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
      NSLOCTEXT("UnrealEd", "Import", "Import").ToString(),
      PackagePath,
      TEXT(""),
      TEXT("Json files (*.json)|*.json|All files (*.*)|*.*"),
      EFileDialogFlags::None,
      OpenFilenames);

   if (bOpened == false)
   {
      return false;
   }
   
   if (OpenFilenames.Num() == 0 || OpenFilenames[0].IsEmpty() == true)
   {
      return false;
   }

   const FString SaveFilename =(*OpenFilenames[0]);
   const TSharedRef<FJsonObject> RootObject = MakeShareable(new FJsonObject);

   FToolBattleCharacterData ExportData;
   const TSharedRef<FJsonObject> OptionsContainer = MakeShareable(new FJsonObject);
   ExportData.EnemyCharacterDatas.Append(NewEnemyCharacterDataList);
   ExportData.PlayerCharacterDatas.Append(NewPlayerCharacterDataList);
   if (FJsonObjectConverter::UStructToJsonObject(FToolBattleCharacterData::StaticStruct(), &(ExportData), OptionsContainer, 0, 0))
   {
      RootObject->SetField(TEXT("BattleLevelPlacedData"), MakeShareable(new FJsonValueObject(OptionsContainer)));
   }   
   FString JsonStr;
   const TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&JsonStr);
   if (FJsonSerializer::Serialize(RootObject, JsonWriter))
   {
      if (IFileManager::Get().FileExists(*SaveFilename))
      {
         IFileManager::Get().Delete(*SaveFilename);
      }
      return FFileHelper::SaveStringToFile(JsonStr, *SaveFilename);
   }
   return false;
}

bool UBattleSimulatorEditorWidget::LoadFromJson()
{
   const FString PPConvertPath = FPaths::GetRootContentDir();
   const FString PackageNameSuggestion = FString(TEXT("/Game/")) + PPConvertPath + FString(TEXT("/"));

   FString PackagePath;
   FPackageName::TryConvertFilenameToLongPackageName(PackageNameSuggestion, PackagePath);
   IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
   if (DesktopPlatform == nullptr)
   {
      return false;
   }
   TArray<FString> OpenFilenames;
   bool bOpened = DesktopPlatform->OpenFileDialog(
      FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
      NSLOCTEXT("UnrealEd", "Import", "Import").ToString(),
      PackagePath,
      TEXT(""),
      TEXT("Json files (*.json)|*.json|All files (*.*)|*.*"),
      EFileDialogFlags::None,
      OpenFilenames);

   if (bOpened == false)
   {
      return false;
   }
   if (OpenFilenames.Num() == 0 || OpenFilenames[0].IsEmpty())
   {
      return false;
   }

   FString JsonStr;
   FFileHelper::LoadFileToString(JsonStr, *OpenFilenames[0]);
   if (JsonStr.IsEmpty())
   {
      return false;
   }

   const TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonStr);
   TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

   if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid() == false)
   {
      return false;
   }
   
   FToolBattleCharacterData Data;
   const TSharedPtr<FJsonValue> OptionsContainer = JsonObject->TryGetField(TEXT("BattleLevelPlacedData"));
   if (OptionsContainer.IsValid())
   {
      FJsonObjectConverter::JsonAttributesToUStruct(OptionsContainer->AsObject()->Values, FToolBattleCharacterData::StaticStruct(), &Data, 0, 0);
   }
    UToolCharacterToolSettingData* PlayerCharacterSettings = UToolCharacterToolSettingData::GetCharacterToolSettingData();
   PlayerCharacterSettings->BattleCharacterData = Data;
   PlayerCharacterSettings->SaveConfigs();
   OnLoadIniDataCompleted(PlayerCharacterSettings);
   return true;

}

void UBattleSimulatorEditorWidget::EnterCmd(const FString& InCommand)
{
   UKismetSystemLibrary::ExecuteConsoleCommand(this,InCommand);
}

bool UBattleSimulatorEditorWidget::OnChangeZoneName(const FString& InZoneName)
{
   TArray<FToolZoneInfo> ZoneInfos = FUPToolTableManager::GetInstance()->GetToolDataPtrByType<FToolZoneInfo>(EToolTableType::ZoneInfo);

   TArray<FToolZoneSpawnInfo> ZoneSpawnInfos = FUPToolTableManager::GetInstance()->GetToolDataPtrByType<FToolZoneSpawnInfo>(EToolTableType::ZoneSpawnInfo);
   
   const int32 FindIndex = ZoneInfos.IndexOfByPredicate([InZoneName](const FToolZoneInfo& Element)
   {
      return Element.ZoneName == InZoneName;
   });
   
   if (FindIndex == INDEX_NONE)
   {
      return false;
   }

   SetZoneAssetName(ZoneInfos[FindIndex].ZoneAssetName);
   
   FString PlayerKey = ZoneInfos[FindIndex].SpawnPlayerKey;
   const int32 PlayerSpawnTableIndex = ZoneSpawnInfos.IndexOfByPredicate([PlayerKey](const FToolZoneSpawnInfo& Element)
   {
      return Element.Name == PlayerKey;
   });
   //
   FString EnemyKey = ZoneInfos[FindIndex].SpawnEnemyKey;
   const int32 EnemySpawnTableIndex = ZoneSpawnInfos.IndexOfByPredicate([EnemyKey](const FToolZoneSpawnInfo& Element)
   {
      return Element.Name == EnemyKey;
   });
   if (EnemySpawnTableIndex == INDEX_NONE)
   {
      return false;
   }
   SetDefaultValueChildList(ZoneSpawnInfos[PlayerSpawnTableIndex], ZoneSpawnInfos[EnemySpawnTableIndex]);
   return true;
}

void UBattleSimulatorEditorWidget::ReadValidCheckInfo()
{
   FInvalidCheckInfo ValidCheckInfo;
   //레벨이름
   ValidCheckInfo.MapNames.Append(GetAllMapNames());

   //스킬아이디
   const TArray<TSharedPtr<FToolSkillInfo>> SkillList =    FUPToolTableManager::GetInstance()->GetToolDataByType<FToolSkillInfo>(EToolTableType::Skill);
   for(const TSharedPtr<FToolSkillInfo>& Skill: SkillList)
   {
      ValidCheckInfo.SKillIDs.Emplace(Skill->Index);
   }
   OnReadValidCheckData(ValidCheckInfo);
}

void UBattleSimulatorEditorWidget::OnSkillPanelSelected(const FToolSkillInfo& InSelectedSkillInfo)
{
}

void UBattleSimulatorEditorWidget::OnSkillToolInfoSelected(const int32 InSelectedSKillIndex)
{
   // 캐릭터 스킬 탭에서의 선택
   OnSkillIndexSelectedDelegate.Broadcast(InSelectedSKillIndex);
}

TArray<FString> UBattleSimulatorEditorWidget::GetAllMapNames()
{
   TArray<FString> MapFiles;

   const TArray<FToolZoneInfo> ZoneInfos = FUPToolTableManager::GetInstance()->GetToolDataPtrByType<FToolZoneInfo>(EToolTableType::ZoneInfo);
   for(const FToolZoneInfo& ZoneInfo : ZoneInfos)
   {
      MapFiles.Emplace(ZoneInfo.ZoneName);
   }
   return MapFiles;
}

void UBattleSimulatorEditorWidget::MakeSkillDetailPanel()
{
   if(SkillEditorWidget.IsValid())
   {
      return;
   }
   
   UObject* Toolbox = UEditorAssetLibrary::LoadAsset(FString(TEXT("/Game/Temp/EditorWidget/SkillPanelEditorWidget")));
   if (IsValid(Toolbox))
   {
      UEditorUtilityWidgetBlueprint* SkillWidgetBlueprint = Cast<UEditorUtilityWidgetBlueprint>(Toolbox);
      if (IsValid(SkillWidgetBlueprint))
      {
         UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
      //   EditorWidget
         EditorUtilitySubsystem->SpawnAndRegisterTab(SkillWidgetBlueprint);
         
         UUPEditorSkillListPanelEditorWidget* GeneratedClass  = Cast<UUPEditorSkillListPanelEditorWidget>(SkillWidgetBlueprint->GetCreatedWidget());
         if(GeneratedClass != nullptr)
         {
            GeneratedClass->GetOnSelectSkillDelegate().AddDynamic(this, &UBattleSimulatorEditorWidget::OnSkillPanelSelected);
            SkillEditorWidget = GeneratedClass;
            OnSkillIndexSelectedDelegate.AddDynamic(SkillEditorWidget.Get(), &UUPEditorSkillListPanelEditorWidget::OnBattleToolSkillIndexSelected);
         }
      }
   }
}

void UBattleSimulatorEditorWidget::BattleStart()
{
   SendDataToJsonTableManager();
   
   const FEditorViewportClient* Client = static_cast<FEditorViewportClient*>(GEditor->GetActiveViewport()->GetClient());
   const FVector editorCameraPosition = Client->GetViewLocation();
   const FRotator editorCameraRotation = Client->GetViewRotation();

   FRequestPlaySessionParams Params;
   Params.StartLocation = editorCameraPosition;
   Params.StartRotation = editorCameraRotation;

   if(Client->GetWorld() != nullptr)
   {
      Client->GetWorld()->GetWorldSettings()->DefaultGameMode = AUPBattleTestGameMode::StaticClass();
   }

   FLevelEditorModule& LevelEditorModule = FModuleManager::Get().GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
   Params.DestinationSlateViewport = LevelEditorModule.GetFirstActiveViewport();
   Params.WorldType = EPlaySessionWorldType::PlayInEditor;
   GEditor->RequestPlaySession(Params);
}

void UBattleSimulatorEditorWidget::SendDataToJsonTableManager()
{
   const FString& LevelName = GetCurrentLevelName();
   FUPDataAssetTableManager::GetInstance()->SetBattleTestZoneName(LevelName);
      
   FUPDeckBattlePlayerData EnemyBattleData;
   EnemyBattleData.CombatDatas.Append(GetDeckBattleUnitData(EBattleTeamType::Enemy));
   FUPDataAssetTableManager::GetInstance()->SetBattleTestEnemyData(EnemyBattleData);
   
   FUPDeckBattlePlayerData PlayerBattleData;
   PlayerBattleData.CombatDatas.Append(GetDeckBattleUnitData(EBattleTeamType::Player));
   FUPDataAssetTableManager::GetInstance()->SetBattleTestPlayerData(PlayerBattleData);

}

TArray<FUPDeckBattleUnitData> UBattleSimulatorEditorWidget::GetDeckBattleUnitData(EBattleTeamType InType) const
{
   TArray<FUPDeckBattleUnitData> CombatDatas;
   const TArray<FToolCharacterData>& ToolDatas = InType == EBattleTeamType::Player ? GetSelectedPlayerDatas() : GetSelectedEnemyDatas();
   CombatDatas.Reserve(ToolDatas.Num());
   
   for (const FToolCharacterData& ToolData : ToolDatas)
   {
      FUPDeckBattleUnitData BattleData;
      BattleData.CharacterType = ToolData.Type;
      BattleData.CharacterTableName = FName(ToolData.Name); // or Name
      BattleData.BattleTeam = InType;
      BattleData.TargetPoint = ToolData.SpawnPosition;
      BattleData.SkillIDs.Append(ToolData.Skill);
      CombatDatas.Emplace(BattleData);
   }
   return CombatDatas;
}

void UBattleSimulatorEditorWidget::OnEndPlayMap()
{
   const FEditorViewportClient* client = static_cast<FEditorViewportClient*>(GEditor->GetActiveViewport()->GetClient());
   if(UPGameHelper::IsBattleTestPlayMode() == true)
   {
      if (client->GetWorld() != nullptr)
      {
         client->GetWorld()->GetWorldSettings()->DefaultGameMode = AUPGameModeBase::StaticClass();
      }      
   }   
}

#undef LOCTEXT_NAMESPACE

