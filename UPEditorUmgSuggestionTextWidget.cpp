
#include "UPEditorUmgSuggestionTextWidget.h"

#include "ToolTable/UPToolTableInfo.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Input/SSuggestionTextBox.h"

void UUPEditorUmgSuggestionTextWidget::NativeConstruct()
{
    Super::NativeConstruct();
    CurrentText.Empty();

    const UToolCharacterToolSettingData* PlayerCharacterSettings = UToolCharacterToolSettingData::GetCharacterToolSettingData();
    CommandList.Empty(PlayerCharacterSettings->CommandList.Num());
    CommandList.Append(PlayerCharacterSettings->CommandList);
}

TSharedRef<SWidget> UUPEditorUmgSuggestionTextWidget::RebuildWidget()
{
    SuggestionTextBox.Reset();
    SuggestionTextBox = SNew(SSuggestionTextBox)
        .SelectAllTextOnCommit(true)
        .OnTextCommitted(FOnTextCommitted::CreateUObject(this, &UUPEditorUmgSuggestionTextWidget::OnTextCommited))
        .OnShowingSuggestions(FOnShowingSuggestions::CreateUObject(this, &UUPEditorUmgSuggestionTextWidget::OnSuggestionTextChanged))
        .SelectAllTextWhenFocused(true)
        .ClearKeyboardFocusOnCommit(true)
        .RevertTextOnEscape(false)
        .TextStyle(FAppStyle::Get(), "Log")
        .HintText(FText::FromString(TEXT("콘솔명령어를 입력하는 창입니다.")));
    if (SuggestionTextBox.IsValid())
    {
        return SuggestionTextBox.ToSharedRef();
    }
    return Super::RebuildWidget();
}

void UUPEditorUmgSuggestionTextWidget::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);
    SuggestionTextBox.Reset();
    CommandList.Empty();
}

void UUPEditorUmgSuggestionTextWidget::OnSuggestionTextChanged(const FString& InText, TArray<FString>& OutSuggestions)
{
    CurrentText = InText;
    for (const FString& s : CommandList)
    {
        if (s.Find(InText) == 0)
        {
            OutSuggestions.Emplace(s);
        }
    }
}

FText UUPEditorUmgSuggestionTextWidget::GetCommandText()
{
    if (SuggestionTextBox == nullptr)
    {
        return FText();
    }
    return SuggestionTextBox->GetText();
}

