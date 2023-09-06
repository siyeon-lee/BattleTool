#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EditlorSkillToolSlate.generated.h"

class UEditorSkillInfo;
class SEditorSkillToolDetailView;
class SEditorSkillToolListProperty;
class UEditorUtilityWidgetBlueprint;
struct FToolSkillInfo;

UCLASS()
class UEditlorSkillToolSlate : public UUserWidget
{
public:

	GENERATED_BODY()
		virtual void NativeConstruct() override;
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	TSharedRef<SWidget> MakeSkillListPanel();
	void ForceSelect(const int32 InSelectIndex);
	void UpdateSkillDetailView(UObject* InUpdateObject, const FToolSkillInfo& InSelected);
	TWeakPtr<SEditorSkillToolListProperty> GetSkillListProperty() const { return SkillListProperty; }
	TWeakPtr<IDetailsView> GetSkillDetailView() const { return SkillDetailsView; }
private:
	void MakeSkillListProperty();
	void MakeSkillDetailView();

private:
	TSharedPtr<SHorizontalBox> HorizontalProperty;
	TSharedPtr<SEditorSkillToolListProperty> SkillListProperty;
	TSharedPtr<IDetailsView> SkillDetailsView;
};
