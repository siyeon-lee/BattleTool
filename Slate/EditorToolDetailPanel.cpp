#include "EditorToolDetailPanel.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "ToolTable/UPToolTableInfo.h"
#define LOCTEXT_NAMESPACE "EditorTool"

FEditorToolDetailPanel::FEditorToolDetailPanel(const FToolSkillInfo& InInfo)
{
    SaveInfo = InInfo;
}

TSharedRef<IDetailCustomization> FEditorToolDetailPanel::MakeInstance(const FToolSkillInfo& InInfo)
{
    return MakeShareable(new FEditorToolDetailPanel(InInfo));
}

void FEditorToolDetailPanel::CustomizeDetails(const TSharedPtr<IDetailLayoutBuilder>& DetailBuilder)
{
    IDetailCustomization::CustomizeDetails(DetailBuilder);
}

void FEditorToolDetailPanel::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& EditorDetailCategory = DetailBuilder.EditCategory("SkillInfo", LOCTEXT("FToolSkillInfo", "ToolSkillInfo"));
    TArray<TWeakObjectPtr<UObject>> ObjectBeginCustomize;
    DetailBuilder.GetObjectsBeingCustomized(ObjectBeginCustomize);
    TArray< TWeakObjectPtr<UObject> > SelectedObjects = DetailBuilder.GetSelectedObjects();
    TArray<TSharedPtr<IPropertyHandle>> ArrayPropertyHandles;
    ArrayPropertyHandles.Add(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(FToolSkillInfo, Index)));
    ArrayPropertyHandles.Add(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(FToolSkillInfo, Name)));
    ArrayPropertyHandles.Add(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(FToolSkillInfo, SkillName_Text)));
    ArrayPropertyHandles.Add(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(FToolSkillInfo, SeqTableNameID)));
    ArrayPropertyHandles.Add(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(FToolSkillInfo, ActAsset)));


    for (const TSharedPtr<IPropertyHandle>& PropertyHandle : ArrayPropertyHandles)
    {
        if (PropertyHandle.IsValid() && PropertyHandle->AsArray().IsValid())
        {
            const TSharedRef<FDetailArrayBuilder> ArrayBuilder = MakeShareable(new FDetailArrayBuilder(PropertyHandle.ToSharedRef()));
            EditorDetailCategory.AddCustomBuilder(ArrayBuilder);
        }
    }
}


#undef LOCTEXT_NAMESPACE
