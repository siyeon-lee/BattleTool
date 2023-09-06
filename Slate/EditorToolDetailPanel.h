#pragma once
#include "IDetailCustomization.h"
#include "ToolTable/UPToolTableInfo.h"

struct FToolSkillInfo;




class FEditorToolDetailPanel : public IDetailCustomization
{
public:
	FEditorToolDetailPanel(const FToolSkillInfo& InInfo);
	static TSharedRef<IDetailCustomization> MakeInstance(const FToolSkillInfo& InInfo);

	// IDetailCustomization
	virtual void CustomizeDetails(const TSharedPtr<IDetailLayoutBuilder>& DetailBuilder) override;
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
private:
	FToolSkillInfo SaveInfo;
};
