#pragma once
#include "CoreMinimal.h"
#include "EditorSkillToolListProperty.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "UPEditorSkillListPanelEditorWidget.generated.h"

class UEditorSkillInfo;
class SEditorSkillToolDetailView;
class SEditorSkillToolListProperty;
class UEditorUtilityWidgetBlueprint;
struct FToolSkillInfo;
class UEditlorSkillToolSlate;

UCLASS()
class UUPEditorSkillListPanelEditorWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
public:
	UUPEditorSkillListPanelEditorWidget(const FObjectInitializer& ObjectInitializer);
	virtual ~UUPEditorSkillListPanelEditorWidget() override;

	virtual bool Initialize() override;
	virtual void Run_Implementation();
	//
	FOnSelectSkill& GetOnSelectSkillDelegate();

	UFUNCTION(BlueprintCallable)
		void UpdateSkillDetailView(const FToolSkillInfo& InSelected);
	UFUNCTION()
		void OnBattleToolSkillIndexSelected(const int32 InSelectIndex);

	UFUNCTION(BlueprintCallable)
		void OnClick_OpenActToolButton(bool bInUseCharacterInfo);
private:
	bool SendDataToActEditor();
	const FString GetHaveSkillCharacterList() const;
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UEditlorSkillToolSlate* SkillSlateWidget;

private:
	TWeakPtr<SEditorSkillToolListProperty> SkillListProperty;
	TWeakPtr<IDetailsView> SkillDetailsView;
	UPROPERTY(Transient)
		TObjectPtr<UEditorSkillInfo> SelectedDetailSkillInfoPtr;

	FToolSkillInfo SelectedSkillInfo;
};
