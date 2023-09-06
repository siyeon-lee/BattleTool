#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "EditorSkillTabPanelWidget.generated.h"
struct FToolSkillInfo;
class UEditableTextBox;

UCLASS()
class UEditorSkillTabPanelWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual bool Initialize() override;

	UFUNCTION(BlueprintCallable)
		void OnClickCopyButton(const TArray<int32>& InSKillList);

	UFUNCTION(BlueprintCallable)
		const TArray<int32> GetClipboardInfo();

	UFUNCTION(BlueprintCallable)
		FToolSkillInfo FindSkillInfo(int32 InSkillIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintImplementableEvent)
		TArray<int> GetCurrentSkillIndexs();
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<FToolSkillInfo> TableSkillInfos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UEditableTextBox* CurrentEditingTextBox;
};
