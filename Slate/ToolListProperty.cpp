

#include "ToolListProperty.h"

//#include "Slate/ListView/UPActName.h"

#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Input/STextComboPopup.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBox.h"

#include "HAL/PlatformApplicationMisc.h"


#define LOCTEXT_NAMESPACE "UPActName"

const FName SToolNameTableRow::ColumnID_UPActNameLabel("UPAct Name");

void SToolNameTableRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
    bCheckBoxView = false;
    bLock = false;

    NameItem = InArgs._SkillNameItem;
    ListProperty = InArgs._ShowSkillListProperty;
    TreeView = InArgs._SkillTreeView;

    check(NameItem.IsValid());

    SMultiColumnTableRow<FToolNameDisplayedPtr>::Construct(FSuperRowType::FArguments(), InOwnerTableView);
}

TSharedRef<SWidget> SToolNameTableRow::GenerateWidgetForColumn(const FName& ColumnName)
{
    if (ColumnName == ColumnID_UPActNameLabel)
    {
        TSharedPtr<SHorizontalBox> RowBox;

        SAssignNew(RowBox, SHorizontalBox);

        RowBox->AddSlot()
            .AutoWidth()
            [
                SNew(SExpanderArrow, SharedThis(this))
            ];

        if (bCheckBoxView)
        {
            RowBox->AddSlot()
                .AutoWidth()
                [
                    SNew(SCheckBox)
                    .IsChecked(this, &SToolNameTableRow::HandleLockCheckBoxIsChecked)
                .OnCheckStateChanged(this, &SToolNameTableRow::HandleLockCheckBoxCheckStateChanged)
                .ToolTipText(LOCTEXT("Lock", "Editor Lock"))
                ];
        }

        NameItem->GenerateWidgetForNameColumn(RowBox, ListProperty->GetFilterText(), FIsSelected::CreateSP(this, &STableRow::IsSelectedExclusively));

        return RowBox.ToSharedRef();
    }

    return SNullWidget::NullWidget;
}

ECheckBoxState SToolNameTableRow::HandleLockCheckBoxIsChecked() const
{
    return bLock ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SToolNameTableRow::HandleLockCheckBoxCheckStateChanged(ECheckBoxState NewState)
{
    if (NewState == ECheckBoxState::Checked)
    {
        bLock = true;
    }
    else
    {
        bLock = false;
    }
}

#undef LOCTEXT_NAMESPACE

/**
 * @brief //
 */
#define LOCTEXT_NAMESPACE "ToolListProperty"

SToolListProperty::SToolListProperty()
{
    LabelName = TEXT("SkillList");
    bShowSearchBox = false;
    bPage = false;

    CurrentPage = 0;
    ViewMaxCount = 100;
    CurrentViewCount = 0;
    CurrentMaxCount = 0;
    MaxPage = 10;
    bTreeViewScrollHorizontal = false;
    SelectionMode = ESelectionMode::Single;

}

SToolListProperty::~SToolListProperty()
{
    //UPActEditorPtr.Reset();

    TreeViewDisplayedRowType.Reset();
    NameFilterBox.Reset();

    NameList.Empty();
    DisplayMirror.Empty();
}


void SToolListProperty::Construct(const FArguments& InArgs)
{
    bLeftCtrl = false;

    //TSharedPtr<SVerticalBox> ContentBox = SNew(SVerticalBox);
    ContentBox = SNew(SVerticalBox);
    ChildSlot
        [
            ContentBox.ToSharedRef()
        ];

    TopSlot();

    if (bShowSearchBox)
    {
        ContentBox->AddSlot().AutoHeight().Padding(FMargin(0.0f, 0.0f, 0.0f, 4.0f))
            [
                SAssignNew(NameFilterBox, SSearchBox)
                .SelectAllTextWhenFocused(true)
            .OnTextChanged(this, &SToolListProperty::OnFilterTextChanged)
            .HintText(FText::FromString(TEXT("Search List")))
            .InitialText(FilterText)
            ];
    }

    if (bTreeViewScrollHorizontal)
    {
        ContentBox->AddSlot().Padding(FMargin(0.0f, 0.0f, 0.0f, 4.0f))
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
            .Padding(FMargin(0.0f, 0.0f, 0.0f, 4.0f))
            [
                SNew(SScrollBox)
                .Orientation(EOrientation::Orient_Horizontal)
            + SScrollBox::Slot()
            [
                SAssignNew(TreeViewDisplayedRowType, ToolDisplayedRowType)
                .SelectionMode(SelectionMode)
            .TreeItemsSource(&NameList)
            .OnGenerateRow(this, &SToolListProperty::GenerateSkillNameRow)
            .OnGetChildren(this, &SToolListProperty::GetChildrenForInfo)
            .OnSelectionChanged(this, &SToolListProperty::OnListItemSelectionChanged)
            .OnMouseButtonDoubleClick(this, &SToolListProperty::OnListItemMouseButtonDoubleClick)
            .ItemHeight(22.0f)
            .HeaderRow
            (
                SNew(SHeaderRow)
                + SHeaderRow::Column(SToolNameTableRow::ColumnID_UPActNameLabel)
                .DefaultLabel(this, &SToolListProperty::GetLabelName)
            )
            ]
            ]
            ];
    }
    else
    {

        ContentBox->AddSlot().Padding(FMargin(0.0f, 0.0f, 0.0f, 4.0f))
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
            .Padding(FMargin(0.0f, 0.0f, 0.0f, 4.0f))
            [
                SAssignNew(TreeViewDisplayedRowType, ToolDisplayedRowType)
                .SelectionMode(SelectionMode)
            .TreeItemsSource(&NameList)
            .OnGenerateRow(this, &SToolListProperty::GenerateSkillNameRow)
            .OnGetChildren(this, &SToolListProperty::GetChildrenForInfo)
            .OnSelectionChanged(this, &SToolListProperty::OnListItemSelectionChanged)
            .OnMouseButtonDoubleClick(this, &SToolListProperty::OnListItemMouseButtonDoubleClick)
            .ItemHeight(22.0f)
            .HeaderRow
            (
                SNew(SHeaderRow)
                + SHeaderRow::Column(SToolNameTableRow::ColumnID_UPActNameLabel)
                .DefaultLabel(this, &SToolListProperty::GetLabelName)
            )
            ]
            ];
    }

    BottomSlot();

    if (bPage)
    {
        ContentBox->AddSlot().AutoHeight().Padding(FMargin(0.0f, 0.0f, 0.0f, 4.0f))
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SButton)
                .ButtonStyle(FAppStyle::Get(), TEXT("RoundButton"))
            .ToolTipText(LOCTEXT("PrevTooltip", "Prev"))
            .OnClicked(this, &SToolListProperty::OnPrevButton)
            .Content()
            [
                SNew(STextBlock)
                .Text(LOCTEXT("PrevButton", "<"))
            .TextStyle(FAppStyle::Get(), TEXT("LargeText"))
            .Justification(ETextJustify::Center)
            ]
            ]
        + SHorizontalBox::Slot()
            .AutoWidth()
            [
                //page
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(FMargin(4, 4, 0, 0))
            [
                SNew(STextBlock)
                .Text(LOCTEXT("Page", "Page: "))
            ]
        + SHorizontalBox::Slot()
            .FillWidth(1)
            .Padding(FMargin(4, 4, 0, 0))
            [
                SNew(STextBlock)
                .Text(this, &SToolListProperty::GetCurrentPage)
            ]
            ]
        + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SButton)
                .ButtonStyle(FAppStyle::Get(), TEXT("RoundButton"))
            .ToolTipText(LOCTEXT("NextTooltip", "Next"))
            .OnClicked(this, &SToolListProperty::OnNextButton)
            .Content()
            [
                SNew(STextBlock)
                .Text(LOCTEXT("NextButton", ">"))
            .TextStyle(FAppStyle::Get(), TEXT("LargeText"))
            .Justification(ETextJustify::Center)
            ]
            ]

        + SHorizontalBox::Slot()
            .AutoWidth()
            [
                //Gopage edittext            
                SNew(SEditableTextBox)
                .Text(this, &SToolListProperty::GetPage)
            .OnTextCommitted(this, &SToolListProperty::OnPageTextCommited)
            .OnTextChanged(this, &SToolListProperty::OnPageTextCommited, ETextCommit::Default)
            ]

        + SHorizontalBox::Slot()
            .AutoWidth()
            [
                //Gopage
                SNew(SButton)
                .ButtonStyle(FAppStyle::Get(), TEXT("RoundButton"))
            .ToolTipText(LOCTEXT("PageTooltip", "GoPage"))
            .OnClicked(this, &SToolListProperty::OnGoPageButton)
            .Content()
            [
                SNew(STextBlock)
                .Text(LOCTEXT("GoPageButton", "GoPage"))
            .TextStyle(FAppStyle::Get(), TEXT("LargeText"))
            .Justification(ETextJustify::Center)
            ]
            ]
        + SHorizontalBox::Slot()
            .FillWidth(1)
            .Padding(FMargin(4, 4, 0, 0))
            [
                SNew(STextBlock)
                .Text(this, &SToolListProperty::GetViewPageCount)
            ]

            ];
    }
}

TSharedRef<ITableRow> SToolListProperty::GenerateSkillNameRow(TSharedPtr<FToolNameDisplayed> InInfo, const TSharedRef<STableViewBase>& OwnerTable)
{
    check(InInfo.IsValid());

    return SNew(SToolNameTableRow, OwnerTable)
        .SkillNameItem(InInfo)
        .ShowSkillListProperty(this);
}

void SToolListProperty::GetChildrenForInfo(TSharedPtr<FToolNameDisplayed> InInfo, TArray<TSharedPtr<FToolNameDisplayed>>& OutChildren)
{
    check(InInfo.IsValid());

    OutChildren = InInfo->Children;
}

void SToolListProperty::OnListItemSelectionChanged(TSharedPtr<FToolNameDisplayed> Item, ESelectInfo::Type SelectInfo)
{
    /*if (true == ShowSkillViewEditorPtr.IsValid())
    {
       if (ESelectInfo::OnMouseClick == SelectInfo)
       {

       }
    }*/
    if (true == Item.IsValid())
    {
        if (Item->Name != NAME_None)
        {
            SelectName = Item->Name;
            SelectCopyName = Item->GetClipCopy();
            Select(Item, SelectName);
        }
    }
    else
    {
        SelectName = NAME_None;
        SelectCopyName = TEXT("");
        TreeViewDisplayedRowType->ClearSelection();
    }
}

void SToolListProperty::OnListItemMouseButtonDoubleClick(TSharedPtr<FToolNameDisplayed> Item)
{

}

void SToolListProperty::Select(TSharedPtr<FToolNameDisplayed> Item, FName BoneName)
{
    //DeselectAll();

    if (true == TreeViewDisplayedRowType.IsValid())
    {
        /*for (int32 Index = 0; Index < DisplayMirror.Num(); ++Index)
        {
           TSharedPtr<FToolNameDisplayed> DisplayedBoneInfo = DisplayMirror[Index];

           if (true == DisplayedBoneInfo.IsValid())
           {
              if (DisplayedBoneInfo->BoneName == BoneName)
              {
                 ShowSkillDisplayedRowType->SetSelection(DisplayedBoneInfo);
              }
           }
        }      */
    }
}

void SToolListProperty::DeselectAll()
{
    if (true == TreeViewDisplayedRowType.IsValid())
    {
        TreeViewDisplayedRowType->ClearSelection();
    }
}

void SToolListProperty::Refresh()
{
    PreRefresh();
    UpdateRefresh();
    PostRefresh();
}

void SToolListProperty::PreRefresh()
{
}

void SToolListProperty::UpdateRefresh()
{
}

void SToolListProperty::PostRefresh()
{
    if (CurrentViewCount > ViewMaxCount)
    {
        CurrentViewCount = ViewMaxCount;
    }

    /*if (FilterText.IsEmpty() == false)
    {
       if (CurrentViewCount == ViewMaxCount)
       {
          MaxPage = CurrentPage + 1;
       }
       else
       {
          MaxPage = CurrentPage;
       }
    }*/

    TreeViewDisplayedRowType->RequestTreeRefresh();
}

void SToolListProperty::OnFilterTextChanged(const FText& SearchText)
{
    if (FilterText.EqualTo(SearchText) == false)
    {
        CurrentPage = 0;
    }

    FilterText = SearchText;

    Refresh();
}

FText& SToolListProperty::GetFilterText()
{
    return FilterText;
}

FReply SToolListProperty::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        ModifyBone();
        return FReply::Handled();
    }

    return FReply::Unhandled();
}

FReply SToolListProperty::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::LeftControl)
    {
        bLeftCtrl = true;
        return FReply::Handled();
    }

    return FReply::Unhandled();
}

FReply SToolListProperty::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
    if (bLeftCtrl && InKeyEvent.GetKey() == EKeys::C)
    {
        TArray<TSharedPtr<class FToolNameDisplayed>> SelectedItemArray = TreeViewDisplayedRowType->GetSelectedItems();
        if (SelectedItemArray.Num() > 0)
        {
            FString SelectCopyNames;

            for (auto Item : SelectedItemArray)
            {
                SelectCopyNames.Append(Item->GetClipCopy());
                SelectCopyNames.Append(TEXT(" \n"));
            }

            FPlatformApplicationMisc::ClipboardCopy(*SelectCopyNames);
        }
        else
        {
            FPlatformApplicationMisc::ClipboardCopy(*SelectCopyName);
        }
    }

    if (InKeyEvent.GetKey() == EKeys::LeftControl)
    {
        bLeftCtrl = false;
    }

    return FReply::Unhandled();
}


void SToolListProperty::ModifyBone()
{
    TArray<FString> Combos;
    Combos.Add(TEXT("COPY"));

    TSharedRef<STextComboPopup> TextEntry =
        SNew(STextComboPopup)
        .Label(LOCTEXT("AddSection", "New Section Name"))
        .TextOptions(Combos)
        //      .SelectAllTextWhenFocused(true)
        .OnTextChosen(this, &SToolListProperty::PopupResult);

    FSlateApplication::Get().PushMenu(
        AsShared(), // Menu being summoned from a menu that is closing: Parent widget should be k2 not the menu thats open or it will be closed when the menu is dismissed
        FWidgetPath(),
        TextEntry,
        FSlateApplication::Get().GetCursorPos(),
        FPopupTransitionEffect(FPopupTransitionEffect::TypeInPopup)
    );

}

void SToolListProperty::PopupResult(const FString& InText)
{

}

void SToolListProperty::PopupWidget(TSharedRef<SWidget> Content)
{
}


FReply SToolListProperty::OnPrevButton()
{
    CurrentPage--;
    if (CurrentPage <= 0)
    {
        CurrentPage = 0;
    }

    Refresh();

    return FReply::Handled();
}

FReply SToolListProperty::OnNextButton()
{
    CurrentPage++;
    if (CurrentPage >= MaxPage)
    {
        CurrentPage = MaxPage;
    }

    Refresh();
    return FReply::Handled();
}

FReply SToolListProperty::OnGoPageButton()
{
    if (CurrentPage >= MaxPage)
    {
        CurrentPage = MaxPage;
    }
    Refresh();

    return FReply::Handled();
}

FText SToolListProperty::GetPage() const
{
    return FText::Format(LOCTEXT("FractionKeysFmt", "{0} "), FText::AsNumber(CurrentPage));
}

FText SToolListProperty::GetCurrentPage() const
{
    return FText::Format(LOCTEXT("FractionKeysFmt", "{0} / {1}"), FText::AsNumber(CurrentPage), FText::AsNumber(MaxPage));
}

FText SToolListProperty::GetViewPageCount() const
{
    return FText::Format(LOCTEXT("FractionKeysFmt", "ViewCount : {0} / MaxCount : {1}"), FText::AsNumber(CurrentViewCount), FText::AsNumber(CurrentMaxCount));
}

void SToolListProperty::OnPageTextCommited(const FText& InText, ETextCommit::Type InCommitType)
{
    int goPage = FCString::Atoi(*(InText.ToString()));
    if (goPage >= 0 && goPage != CurrentPage)
    {
        if (CurrentPage <= MaxPage)
        {
            CurrentPage = MaxPage;
        }
        Refresh();
    }
}

FText SToolListProperty::GetLabelName() const
{
    return FText::FromString(LabelName);
}

#undef LOCTEXT_NAMESPACE
