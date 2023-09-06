
#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"

#include "Engine/DeveloperSettings.h"

#include "BattleSimulatorEditorWidget.generated.h"

class UComboBoxKey;
struct FZoneSpawnTableItem;
struct FUPDeckBattleUnitData;
class UUPEditorSkillListPanelEditorWidget;
enum class EBattleTeamType : uint8;
class UEditableTextBox;
class UEditorUtilityWidgetBlueprint;


/*
 * BattleSimulatorEditorWidget : 배틀모드로 게임을 시작할 때 관련 설정을 해주는 툴
 *
 * BP 소유 클래스 관계 : 데이터 단위 관계로 분류
 * 
 * BattleSimulatorEditorWidget : 툴의 가장 최상단 에디터유틸리티위젯.
 *   __________________________________________________________
 *   ㄴ BP_CharacterTool : UToolCharacterToolSettingData::TArray<FToolCharacterData> 
 *      ㄴBP_CharacterSimpleInfo : FToolCharacterData( 단, Skill 한정 가장 앞 3개만 노출함)
 *         ㄴ BP_SkillTabInfo : FToolCharacterData::TArray<int32> Skill
 *   ㄴ BP_CommandList : 콘솔명령어 실행 UToolCharacterToolSettingData::CommandList
 *   __________________________________________________________
 *   UUPEditorSkillListPanelEditorWidget : FToolSkillInfo 리스트를 보여주는 별개의 툴.
 *   ----------------------------------------------------------
 *   ㄴ UToolCharacterToolSettingData 위젯들과 별개로
 *   
 *   참고 : 편집했을 시 위젯 단위별로 정보수정이 잦아서, 위젯마다 데이터 PTR을 따로 두지 않고 Get___Data 함수로 위젯 내용을 읽어온다.
 */

USTRUCT(BlueprintType)
struct FInvalidCheckInfo
{
   GENERATED_BODY()
public:
   UPROPERTY(BlueprintReadWrite, EditAnywhere)
   TArray<FString> MapNames;
   UPROPERTY(BlueprintReadWrite, EditAnywhere)
   TArray<int> SKillIDs;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectSkillIndex, const int32, SelectedSkillIndex);


UCLASS()
class UBattleSimulatorEditorWidget : public UEditorUtilityWidget
{
   GENERATED_BODY()
public:
   UBattleSimulatorEditorWidget(const FObjectInitializer& ObjectInitializer);
   virtual ~UBattleSimulatorEditorWidget() override;
public:
   virtual void NativeConstruct() override;
   virtual bool Initialize() override;
   virtual void ReleaseSlateResources(bool bReleaseChildren) override;
   virtual void Run_Implementation();
   UFUNCTION(BlueprintCallable)
   bool ReadTable();

   UFUNCTION(BlueprintCallable)
   bool ReloadTable();

   UFUNCTION(BlueprintImplementableEvent)
   void OnLoadIniDataCompleted(const UToolCharacterToolSettingData* Data);

   UFUNCTION(BlueprintImplementableEvent)
   void OnReadCharacterTable(const TArray<FToolCharacterData>& TableReadList);

   UFUNCTION(BlueprintImplementableEvent)
   void OnReadZoneInfoTable(const TArray<FToolZoneInfo>& TableReadList);

   UFUNCTION(BlueprintImplementableEvent)
   void OnReadZoneSpawnTable(const TArray<FToolZoneSpawnInfo>& TableReadList);

   UFUNCTION(BlueprintImplementableEvent)
   void SetDefaultValueChildList(const FToolZoneSpawnInfo& PlayerSpawnInfo, const FToolZoneSpawnInfo& EnemySpawnInfo);

   UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
   void SetZoneAssetName(const FString& InZoneAssetName);
   
   UFUNCTION(BlueprintImplementableEvent)
   void OnReadValidCheckData(const FInvalidCheckInfo& CheckInfo);

   UFUNCTION(BlueprintCallable)
   void SaveDataToIni(TArray<FToolCharacterData> NewPlayerCharacterDataList, TArray<FToolCharacterData> NewEnemyCharacterDataList , const FString& InZoneID, const FString& InZoneAssetName, const TArray<FString>& InCommandList);
   
   UFUNCTION(BlueprintCallable)
   void LoadFromIni();

   UFUNCTION(BlueprintCallable)
   bool SaveToJson( const TArray<FToolCharacterData>& NewPlayerCharacterDataList, const TArray<FToolCharacterData>& NewEnemyCharacterDataList );

   UFUNCTION(BlueprintCallable)
   bool LoadFromJson();

   UFUNCTION(BlueprintCallable)
   void EnterCmd(const FString& InCommand);

   UFUNCTION(BlueprintCallable)
   bool OnChangeZoneName(const FString& InZoneName);

   UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, BlueprintPure)
   const FString GetCurrentLevelName();

   UFUNCTION(BlueprintImplementableEvent)
   const TArray<FToolCharacterData> GetSelectedPlayerDatas() const;
   
   UFUNCTION(BlueprintImplementableEvent)
   const TArray<FToolCharacterData> GetSelectedEnemyDatas() const;

   UFUNCTION(BlueprintCallable)
   void MakeSkillDetailPanel();
   
   UFUNCTION(BlueprintCallable)
   void OnSkillToolInfoSelected(const int32 InSelectedSKillIndex);

private:
   UFUNCTION(BlueprintCallable)
   void BattleStart();

   void SendDataToJsonTableManager();
   TArray<FUPDeckBattleUnitData> GetDeckBattleUnitData(EBattleTeamType InType) const;
   
   void OnEndPlayMap();
   void ReadValidCheckInfo();

   UFUNCTION()
   void OnSkillPanelSelected(const FToolSkillInfo& InSelectedSkillInfo);

public:
   UPROPERTY(BlueprintReadWrite, EditAnywhere)
   TArray<FToolCharacterData> TableCharacterDatas;

public:
   FOnSelectSkillIndex OnSkillIndexSelectedDelegate;
private:
   TWeakObjectPtr<UUPEditorSkillListPanelEditorWidget> SkillEditorWidget;
   TArray<FString> GetAllMapNames();

   
   int32 CurrentSelectedSkillIndex;   //최상단이 아니라 스킬툴에서 관리해야함.
};
