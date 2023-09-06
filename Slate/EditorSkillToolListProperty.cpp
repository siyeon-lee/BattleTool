

#include "EditorSkillToolListProperty.h"
#include "ToolTable/UPToolTableManager.h"


class FShowSkillNameDisplayed : public FToolNameDisplayed
{
public:
    static TSharedRef<FShowSkillNameDisplayed> Make(const FName& InName, TSharedPtr<FToolSkillInfo> InInfo)
    {
        return MakeShareable(new FShowSkillNameDisplayed(InName, InInfo));
    }

    virtual ~FShowSkillNameDisplayed() override {}

protected:

    FShowSkillNameDisplayed(const FName& InName, const TSharedPtr<FToolSkillInfo>& InInfo)
        : FToolNameDisplayed(InName)
        , SkillInfo(InInfo)
    {
    }

    FShowSkillNameDisplayed() {}

public:
    TSharedPtr<FToolSkillInfo> SkillInfo;

};

void SEditorSkillToolListProperty::Construct(const FArguments& InArgs)
{
    bPage = true;
    bShowSearchBox = true;
    bTreeViewScrollHorizontal = true;
    ViewMaxCount = 100;
    SToolListProperty::Construct(SToolListProperty::FArguments());
}


SEditorSkillToolListProperty::~SEditorSkillToolListProperty()
{

}

void SEditorSkillToolListProperty::UpdateRefresh()
{
    SToolListProperty::UpdateRefresh();
    UpdateSkillDisplayList();
    UpdatePageCountView();
}

void SEditorSkillToolListProperty::UpdateSkillDisplayList()
{
    NameList.Empty();

    int32 count = 0;
    const TArray<TSharedPtr<FToolSkillInfo>> SkillList = FUPToolTableManager::GetInstance()->GetToolDataByType<FToolSkillInfo>(EToolTableType::Skill);

    if (SkillList.Num() <= 0)
    {
        return;
    }

    const int StartIndex = CurrentPage * ViewMaxCount;
    for (int i = StartIndex; i < SkillList.Num(); i++)
    {
        const TSharedPtr<FToolSkillInfo>& SkillData = SkillList[i];
        if (FilterText.IsEmpty() == false)
        {
            if (SkillData->Name.Contains(FilterText.ToString()) == false)
            {
                continue;
            }
        }
        NameList.Emplace(MakeSkillNameDisplay(SkillData));

        count++;

        //최대 표시 갯수를 넘었을 때
        if (count > ViewMaxCount)
        {
            break;
        }
    }
}

void SEditorSkillToolListProperty::UpdatePageCountView()
{
    const TArray<TSharedPtr<FToolSkillInfo>> SkillList = FUPToolTableManager::GetInstance()->GetToolDataByType<FToolSkillInfo>(EToolTableType::Skill);

    if (SkillList.Num() <= 0)
    {
        return;
    }
    MaxPage = SkillList.Num() / ViewMaxCount;
    CurrentMaxCount = SkillList.Num();
    CurrentViewCount = NameList.Num();
}

TSharedRef<FShowSkillNameDisplayed> SEditorSkillToolListProperty::MakeSkillNameDisplay(const TSharedPtr<FToolSkillInfo>& InSkillTableInfo)
{
    FString DisplayName = InSkillTableInfo->Name + TEXT("_") + FString::FromInt(InSkillTableInfo->Index) + TEXT("_") + InSkillTableInfo->SkillName_Text;
    return FShowSkillNameDisplayed::Make(FName(*DisplayName), InSkillTableInfo);
}

void SEditorSkillToolListProperty::Select(TSharedPtr<FToolNameDisplayed> Item, FName InName)
{
    SToolListProperty::Select(Item, InName);
    if (Item.IsValid())
    {
        const TSharedPtr<FShowSkillNameDisplayed> NameData = StaticCastSharedPtr<FShowSkillNameDisplayed>(Item);
        if (NameData.IsValid() == true)
        {
            if (NameData->Name != NAME_None && NameData->SkillInfo.IsValid())
            {
                OnSkillSelected.Broadcast(*NameData->SkillInfo.Get());
            }
        }
    }
}

void SEditorSkillToolListProperty::ForceSelect(const int32 InSKillIndex)
{
    const int32 FindNameListIndex = FindSkillNameDisplayIndexBySkillIndex(InSKillIndex);
    if (FindNameListIndex == INDEX_NONE)
    {
        return;
    }

    SelectName = NameList[FindNameListIndex]->Name;
    SelectCopyName = NameList[FindNameListIndex]->GetClipCopy();
    Select(NameList[FindNameListIndex], NameList[FindNameListIndex]->Name);
    TreeViewDisplayedRowType->SetSelection(NameList[FindNameListIndex], ESelectInfo::Direct);
}


int32 SEditorSkillToolListProperty::FindSkillNameDisplayIndexBySkillIndex(const int32 InSkillIndex) const
{
    const TArray<TSharedPtr<FToolSkillInfo>> SkillList = FUPToolTableManager::GetInstance()->GetToolDataByType<FToolSkillInfo>(EToolTableType::Skill);
    const int32 FindSkillIndex = SkillList.IndexOfByPredicate([InSkillIndex](const TSharedPtr<FToolSkillInfo>& Info)
        {
            return Info->Index == InSkillIndex;
        });

    if (FindSkillIndex == INDEX_NONE)
    {
        return INDEX_NONE;
    }

    const FName FindDisplayName = FName(SkillList[FindSkillIndex]->Name + TEXT("_") + FString::FromInt(SkillList[FindSkillIndex]->Index) + TEXT("_") + SkillList[FindSkillIndex]->SkillName_Text);


    const int32 FindNameListIndex = NameList.IndexOfByPredicate([FindDisplayName](const TSharedPtr<FToolNameDisplayed>& NameDisplayed)
        {
            return NameDisplayed->Name == FindDisplayName;
        });

    return FindNameListIndex;

}
