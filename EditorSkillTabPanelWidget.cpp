#include "EditorSkillTabPanelWidget.h"

#include "HAL/PlatformApplicationMisc.h"
#include "ToolTable/UPToolTableManager.h"
#include "ToolTable/UPToolTableInfo.h"

bool UEditorSkillTabPanelWidget::Initialize()
{
    Super::Initialize();
    TableSkillInfos = FUPToolTableManager::GetInstance()->GetToolDataPtrByType<FToolSkillInfo>(EToolTableType::Skill);
    return true;
}

void UEditorSkillTabPanelWidget::OnClickCopyButton(const TArray<int32>& InSKillList)
{
    FString ClipBoardText;
    for (int i = 0; i < InSKillList.Num(); i++)
    {
        ClipBoardText.Append(FString::FromInt(InSKillList[i]));
        if (i != InSKillList.Num() - 1)
        {
            ClipBoardText.Append("\t");
        }
    }
    FPlatformApplicationMisc::ClipboardCopy(*ClipBoardText);
}

const TArray<int32> UEditorSkillTabPanelWidget::GetClipboardInfo()
{
    FString ClipBoardText;
    FPlatformApplicationMisc::ClipboardPaste(ClipBoardText);
    TArray<int32> RetSKillList;
    TArray<FString> TrimStringList;
    ClipBoardText.ParseIntoArray(TrimStringList, TEXT("\t"));
    for (const FString& TrimStr : TrimStringList)
    {
        if (TrimStr.IsNumeric() == true)
        {
            RetSKillList.Emplace(FCString::Atoi(*TrimStr));
        }
    }
    return RetSKillList;
}

FToolSkillInfo UEditorSkillTabPanelWidget::FindSkillInfo(int32 InSkillIndex)
{
    const int32 FindIndex = TableSkillInfos.IndexOfByPredicate([InSkillIndex](const FToolSkillInfo& Element)
        {
            return Element.Index == InSkillIndex;
        });
    if (FindIndex == INDEX_NONE)
    {
        return FToolSkillInfo();
    }

    return TableSkillInfos[FindIndex];
}
