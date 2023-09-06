
#include "UPEditorSkillListPanelEditorWidget.h"
#include "EditlorSkillToolSlate.h"
#include "EditorSkillToolListProperty.h"
//#include "BattleTool/UPEditorSkillListPanelWidget.h"
#include "ToolTable/UPToolTableInfo.h"

#include "ToolTable/UPToolTableManager.h"

UUPEditorSkillListPanelEditorWidget::UUPEditorSkillListPanelEditorWidget(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    bAutoRunDefaultAction = true;
}

UUPEditorSkillListPanelEditorWidget::~UUPEditorSkillListPanelEditorWidget()
{

}

bool UUPEditorSkillListPanelEditorWidget::Initialize()
{
    Super::Initialize();

    if (SkillSlateWidget == nullptr)
    {
        return false;
    }
    SkillSlateWidget->MakeSkillListPanel();
    SkillSlateWidget->RebuildWidget();
    SkillListProperty = SkillSlateWidget->GetSkillListProperty();
    SkillDetailsView = SkillSlateWidget->GetSkillDetailView();
    SkillListProperty.Pin()->OnSkillSelected.AddDynamic(this, &UUPEditorSkillListPanelEditorWidget::UpdateSkillDetailView);
    UpdateSkillDetailView(FToolSkillInfo());
    return true;
}

void UUPEditorSkillListPanelEditorWidget::Run_Implementation()
{
    //SkillSlateWidget->MakeSkillListPanel();

 //RebuildWidget();
}


FOnSelectSkill& UUPEditorSkillListPanelEditorWidget::GetOnSelectSkillDelegate()
{
    return SkillListProperty.Pin()->OnSkillSelected;
}


void UUPEditorSkillListPanelEditorWidget::OnBattleToolSkillIndexSelected(const int32 InSelectIndex)
{
    if (SkillSlateWidget != nullptr)
    {
        SkillSlateWidget->ForceSelect(InSelectIndex);
    }
}

void UUPEditorSkillListPanelEditorWidget::OnClick_OpenActToolButton(bool bInUseCharacterInfo)
{
    if (SelectedSkillInfo.ActAsset.IsValid() == false)
    {
        return;
    }

    if (bInUseCharacterInfo == true)
    {
        if (SendDataToActEditor() == false)
        {
            UE_LOG(LogTemp, Error, TEXT("No Matching Character Index"));
        }
    }

    //Open Act Tool
    if (UObject* ObjectToEdit = SelectedSkillInfo.ActAsset.TryLoad())
    {
        GEditor->EditObject(ObjectToEdit);
    }
}

bool UUPEditorSkillListPanelEditorWidget::SendDataToActEditor()
{
    FString TargetCharacterAssetName = GetHaveSkillCharacterList();
    if (TargetCharacterAssetName.IsEmpty())
    {
        return false;
    }
    FUPToolTableManager::GetInstance()->SetActToolAssetName(TargetCharacterAssetName);
    return true;
}

const FString UUPEditorSkillListPanelEditorWidget::GetHaveSkillCharacterList() const
{
    const int32 SelectedSkillIndex = SelectedSkillInfo.Index;
    const TArray<TSharedPtr<FToolCharacterData>> CharacterList = FUPToolTableManager::GetInstance()->GetToolDataByType<FToolCharacterData>(EToolTableType::CharacterTable);
    for (const TSharedPtr<FToolCharacterData>& CharacterInfo : CharacterList)
    {
        if (CharacterInfo->Skill.Contains(SelectedSkillIndex) == true)
        {
            return CharacterInfo->CharacterAssetName;
        }
    }

    return TEXT("");
}

void UUPEditorSkillListPanelEditorWidget::UpdateSkillDetailView(const FToolSkillInfo& InSelected)
{
    SelectedSkillInfo = InSelected;
    if (SkillSlateWidget == nullptr)
    {
        return;
    }
    if (SelectedDetailSkillInfoPtr == nullptr)
    {
        SelectedDetailSkillInfoPtr = NewObject<UEditorSkillInfo>();
    }
    SelectedDetailSkillInfoPtr->InitValue(InSelected);

    SkillSlateWidget->UpdateSkillDetailView(SelectedDetailSkillInfoPtr, InSelected);
}
