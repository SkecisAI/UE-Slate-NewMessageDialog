// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class IMessageDialogButtonWidget
{
protected:
	EAppReturnType::Type MsgReturnType = EAppReturnType::Cancel;

	TSharedPtr<SWindow> ParentWindow;

	void CloseWindow()
	{
		if (ParentWindow.IsValid())
		{
			ParentWindow->RequestDestroyWindow();
		}
	}
public:
	virtual ~IMessageDialogButtonWidget() = default;
	virtual TSharedPtr<SWidget> GetDialogWidget() = 0;
	EAppReturnType::Type GetMsgReturnType() const {return MsgReturnType;}
	void SetParentWindow(const TSharedRef<SWindow>& InWindow) {ParentWindow = InWindow;}
};

class FMessageDialogWidget_OneButton : public IMessageDialogButtonWidget
{
public:
	virtual TSharedPtr<SWidget> GetDialogWidget() override;
};

class FMessageDialogWidget_TwoButtons : public IMessageDialogButtonWidget
{
public:
	virtual TSharedPtr<SWidget> GetDialogWidget() override;
};

class FMessageDialogWidget_ThreeButtons : public IMessageDialogButtonWidget
{
public:
	virtual TSharedPtr<SWidget> GetDialogWidget() override;
};

/**
 * 
 */
class NEWMESSAGEDIALOG_API SNewMessageDialog : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SNewMessageDialog)
		{
		}

	SLATE_ARGUMENT(FText, MessageText);
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, TSharedPtr<SWindow> InDialogWindow, EAppMsgType::Type InMsgType);

	void SwitchButtonsWidget();

	static EAppReturnType::Type Open( EAppMsgType::Type MessageType, const FText& Message, const FText* OptTitle = nullptr);

	EAppReturnType::Type GetMsgReturnType() const;

private:
	TSharedPtr<SWindow> DialogWindow;

	EAppMsgType::Type MsgType = EAppMsgType::Ok;

	TSharedPtr<IMessageDialogButtonWidget> ButtonWidget;
};
