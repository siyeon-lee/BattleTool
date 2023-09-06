#pragma once
#include "CoreMinimal.h"
#include "ToolListProperty.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STreeView.h"
#include "ToolTable/UPToolTableInfo.h"

#include "EditorSkillToolListProperty.generated.h"
//#include "EditorSkillInfo.generated.h"
class FShowSkillNameDisplayed;
class SHorizontalBox;
struct FToolSkillInfo;

UCLASS()
class UEditorSkillInfo : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 Index;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString SkillName_Text = TEXT("");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FSoftObjectPath ActAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString SeqTableNameID = TEXT("");

    void InitValue(const FToolSkillInfo& InInfo)
    {
        this->Index = InInfo.Index;
        this->Name = InInfo.Name;
        this->SkillName_Text = InInfo.SkillName_Text;
        this->ActAsset = InInfo.ActAsset;
        this->SeqTableNameID = InInfo.SeqTableNameID;
    }
    //Initializer Ãß°¡
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectSkill, const FToolSkillInfo&, SelectedSkillInfo);

class SEditorSkillToolListProperty : public SToolListProperty
{
    SLATE_BEGIN_ARGS(SEditorSkillToolListProperty) {}
    //SLATE_ARGUMENT(TSharedPtr<class FUPActEditor>, UPActEditor)
    SLATE_END_ARGS()

public:
    virtual void Construct(const FArguments& InArgs);
    virtual ~SEditorSkillToolListProperty() override;
    virtual void Select(TSharedPtr<FToolNameDisplayed> Item, FName InName) override;
    void ForceSelect(const int32 InSKillIndex);
protected:
    virtual void UpdateRefresh() override;
private:
    void UpdateSkillDisplayList();
    void UpdatePageCountView();
    TSharedRef<FShowSkillNameDisplayed> MakeSkillNameDisplay(const TSharedPtr<FToolSkillInfo>& InSkillTableInfo);
    int32 FindSkillNameDisplayIndexBySkillIndex(const int32 InSkillIndex) const;

public:
    FOnSelectSkill OnSkillSelected;

};
