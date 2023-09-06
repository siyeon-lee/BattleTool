#include "EditlorSkillToolSlate.h"

#include "EditorSkillToolListProperty.h"
#include "EditorToolDetailPanel.h"

void UEditlorSkillToolSlate::NativeConstruct()
{
    Super::NativeConstruct();
    MakeSkillListPanel();
}

TSharedRef<SWidget> UEditlorSkillToolSlate::RebuildWidget()
{
    MakeSkillListPanel();
    if (HorizontalProperty.IsValid() == true)
    {
        return HorizontalProperty.ToSharedRef();
    }
    return Super::RebuildWidget();
}

void UEditlorSkillToolSlate::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);
    if (SkillListProperty.IsValid())
    {
        SkillListProperty.Reset();
    }

    if (SkillDetailsView.IsValid())
    {
        SkillDetailsView.Reset();
    }
    if (HorizontalProperty.IsValid())
    {
        HorizontalProperty.Reset();
    }
}

TSharedRef<SWidget> UEditlorSkillToolSlate::MakeSkillListPanel()
{
    if (HorizontalProperty.IsValid() == true)
    {
        return HorizontalProperty.ToSharedRef();
    }
    MakeSkillListProperty();
    MakeSkillDetailView();

    if (SkillDetailsView.IsValid())
    {
        //텅 빈 스킬 디테일뷰 업데이트

    }

    // 가로박스에 스킬리스트, 스킬디테일뷰 묶기
    HorizontalProperty = SNew(SHorizontalBox)
        + SHorizontalBox::Slot().Padding(5).AutoWidth()
        [
            SkillListProperty.ToSharedRef()
        ]
    + SHorizontalBox::Slot().Padding(5).AutoWidth()
        [
            SkillDetailsView.ToSharedRef()
        ];

    return HorizontalProperty.ToSharedRef();
}

void UEditlorSkillToolSlate::ForceSelect(const int32 InSelectIndex)
{
    if (SkillListProperty.IsValid())
    {
        SkillListProperty->ForceSelect(InSelectIndex);
    }
}

void UEditlorSkillToolSlate::MakeSkillListProperty()
{
    if (SkillListProperty.IsValid())
    {
        SkillListProperty.Reset();
    }
    SkillListProperty = SNew(SEditorSkillToolListProperty);
    SkillListProperty->Refresh();
}

void UEditlorSkillToolSlate::MakeSkillDetailView()
{
    if (SkillDetailsView.IsValid())
    {
        SkillDetailsView.Reset();
    }
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bAllowSearch = true;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    DetailsViewArgs.bAllowMultipleTopLevelObjects = true;
    DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Show;
    SkillDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs).ToSharedPtr();

}

void UEditlorSkillToolSlate::UpdateSkillDetailView(UObject* InUpdateObject, const FToolSkillInfo& InSelected)
{
    if (SkillDetailsView == nullptr)
    {
        return;
    }
    SkillDetailsView->SetObject(InUpdateObject, false);
    SkillDetailsView->RegisterInstancedCustomPropertyLayout(
        UEditorSkillInfo::StaticClass(), FOnGetDetailCustomizationInstance::CreateLambda([=]()
            { return FEditorToolDetailPanel::MakeInstance(InSelected); }));
}
