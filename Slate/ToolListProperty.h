#pragma once

class FToolNameDisplayed
{
public:

    static TSharedRef<FToolNameDisplayed> Make(const FName& InName)
    {
        return MakeShareable(new FToolNameDisplayed(InName));
    }

    virtual void GenerateWidgetForNameColumn(TSharedPtr<SHorizontalBox> Box, FText& FilterText, FIsSelected InIsSelected)
    {
        Box->AddSlot()
            .AutoWidth()
            .Padding(2, 0, 0, 0)
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(FText::FromName(Name))
            .HighlightText(FilterText)
            ];
    }

    virtual ~FToolNameDisplayed() {}

    virtual FString GetClipCopy() { return Name.ToString(); }
protected:

    FToolNameDisplayed(const FName& InName)
        : Name(InName)
    {
    }

    FToolNameDisplayed() {}

public:

    FName Name;

    TArray<TSharedPtr<FToolNameDisplayed>> Children;
};
typedef STreeView<TSharedPtr<FToolNameDisplayed>> ToolDisplayedRowType;

typedef TSharedPtr<FToolNameDisplayed> FToolNameDisplayedPtr;
class SToolNameTableRow : public SMultiColumnTableRow<FToolNameDisplayedPtr>
{
public:

    SLATE_BEGIN_ARGS(SToolNameTableRow) {}
    SLATE_ARGUMENT(FToolNameDisplayedPtr, SkillNameItem)
        SLATE_ARGUMENT(class SToolListProperty*, ShowSkillListProperty)
        SLATE_ARGUMENT(TSharedPtr<ToolDisplayedRowType>, SkillTreeView)
        SLATE_END_ARGS()

        void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView);
    virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;

    ECheckBoxState HandleLockCheckBoxIsChecked() const;
    void HandleLockCheckBoxCheckStateChanged(ECheckBoxState NewState);

public:
    static const FName ColumnID_UPActNameLabel;

private:
    bool bCheckBoxView;
    bool bLock;
    class SToolListProperty* ListProperty;
    FToolNameDisplayedPtr            NameItem;
    TSharedPtr<ToolDisplayedRowType>   TreeView;
};

class SToolListProperty : public SCompoundWidget
{
public:

    SLATE_BEGIN_ARGS(SToolListProperty) {}
    //SLATE_ARGUMENT(TSharedPtr<class FUPActEditor>, UPActEditor)
    SLATE_END_ARGS()

        void Construct(const FArguments& InArgs);

    SToolListProperty();
    virtual ~SToolListProperty();

    void Refresh();

    FText& GetFilterText();

    virtual void Select(TSharedPtr<class FToolNameDisplayed> Item, FName BoneName);
    virtual void DeselectAll();

    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
    virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

protected:

    TSharedRef<ITableRow> GenerateSkillNameRow(TSharedPtr<class FToolNameDisplayed> InInfo, const TSharedRef<STableViewBase>& OwnerTable);

    void GetChildrenForInfo(TSharedPtr<class FToolNameDisplayed> InInfo, TArray< TSharedPtr<class FToolNameDisplayed> >& OutChildren);

    void ModifyBone();
    void PopupWidget(TSharedRef<SWidget> Content);
    void PopupResult(const FString& InText);

    virtual void PreRefresh();
    virtual void UpdateRefresh();
    virtual void PostRefresh();

protected:

    virtual void TopSlot() {}
    virtual void BottomSlot() {}
    virtual void OnFilterTextChanged(const FText& SearchText);

    virtual void OnListItemSelectionChanged(TSharedPtr<class FToolNameDisplayed> Item, ESelectInfo::Type SelectInfo);
    virtual void OnListItemMouseButtonDoubleClick(TSharedPtr<class FToolNameDisplayed> Item);

    FReply OnPrevButton();
    FReply OnNextButton();
    FText GetCurrentPage() const;
    FText GetViewPageCount() const;
    FReply OnGoPageButton();
    FText GetPage() const;
    void OnPageTextCommited(const FText& InText, ETextCommit::Type InCommitType);

    virtual FText GetLabelName() const;

protected:

    FText   FilterText;

    bool bLeftCtrl;

    TSharedPtr<class SSearchBox>            NameFilterBox;
    TSharedPtr<STreeView<TSharedPtr<class FToolNameDisplayed>>>   TreeViewDisplayedRowType;

    TArray<TSharedPtr<class FToolNameDisplayed>>   NameList;
    TArray<TSharedRef<class FToolNameDisplayed>>   DisplayMirror;

    FName SelectName;
    FString SelectCopyName;

    FString LabelName;

    TSharedPtr<class SVerticalBox> ContentBox;
    bool bShowSearchBox;

    FDelegateHandle ShowChangedHandle;

    bool bPage;
    int32 ViewMaxCount;
    int32 CurrentPage;
    int32 MaxPage;
    int32 CurrentViewCount;
    int32 CurrentMaxCount;

    ESelectionMode::Type SelectionMode;
    bool bTreeViewScrollHorizontal;

    //int32 FindPageStartIndex;
    //TArray<int32> PageStartIndexs;
};
