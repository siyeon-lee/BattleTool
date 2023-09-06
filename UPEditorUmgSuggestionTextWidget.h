#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "UPEditorUmgSuggestionTextWidget.generated.h"

class SWidget;
class SUPSuggestionTextBox;

UCLASS()
class UUPEditorUmgSuggestionTextWidget : public UUserWidget
{
public:
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	void OnSuggestionTextChanged(const FString& Text, TArray<FString>& OutSuggestions);

	UFUNCTION(BlueprintImplementableEvent)
		void OnTextCommited(const FText& InText, ETextCommit::Type InType);

	UFUNCTION(BlueprintCallable)
		FText GetCommandText();
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString CurrentText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FString> CommandList;

private:
	TSharedPtr<SSuggestionTextBox> SuggestionTextBox;
};
