// Fill out your copyright notice in the Description page of Project Settings.


#include "SNewMessageDialog.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "NewMessageDialog"

constexpr uint8 CHINESE_CHAR_WIDTH = 12;
constexpr uint8 ALPHA_CHAR_WIDTH = 5;

TSharedPtr<SWidget> FMessageDialogWidget_OneButton::GetDialogWidget()
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(5)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		[
			SNew(SButton)
			.Text(LOCTEXT("Ok_Text", "Ok"))
			.OnClicked(FOnClicked::CreateLambda([this]()
			{
				MsgReturnType = EAppReturnType::Ok;
				CloseWindow();
				return FReply::Handled();
			}))
		];
	
}

TSharedPtr<SWidget> FMessageDialogWidget_TwoButtons::GetDialogWidget()
{
	return SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.AutoWidth()
	.Padding(5)
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	[
		SNew(SButton)
		.Text(LOCTEXT("Yes_Text", "Yes"))
		.OnClicked(FOnClicked::CreateLambda([this]()
		{
			MsgReturnType = EAppReturnType::Yes;
			CloseWindow();
			return FReply::Handled();
		}))
	]
	+ SHorizontalBox::Slot()
	.AutoWidth()
	.Padding(5)
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	[
		SNew(SButton)
		.Text(LOCTEXT("No_Text", "No"))
		.OnClicked(FOnClicked::CreateLambda([this]()
		{
			MsgReturnType = EAppReturnType::No;
			CloseWindow();
			return FReply::Handled();
		}))
	];
}

void SNewMessageDialog::Construct(const FArguments& InArgs, TSharedPtr<SWindow> InDialogWindow, EAppMsgType::Type InMsgType)
{
	DialogWindow = InDialogWindow;
	MsgType = InMsgType;
	
	SwitchButtonsWidget();
	
	ChildSlot
	[
		// Populate the widget
		SNew(SBorder)
		.Padding(2)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Text(InArgs._MessageText)
			]
			+ SOverlay::Slot()
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Center)
			[
				ButtonWidget->GetDialogWidget().ToSharedRef()
			]
		]
	];
}

void SNewMessageDialog::SwitchButtonsWidget()
{
	ButtonWidget.Reset();
	
	switch (MsgType)
	{
	case EAppMsgType::Ok:
		ButtonWidget = MakeShareable(new FMessageDialogWidget_OneButton);
		break;
	case EAppMsgType::YesNo:
		ButtonWidget = MakeShareable(new FMessageDialogWidget_TwoButtons);
		break;
	case EAppMsgType::OkCancel:
		ButtonWidget = MakeShareable(new FMessageDialogWidget_TwoButtons);
		break;
	case EAppMsgType::YesNoCancel:
		break;
	case EAppMsgType::CancelRetryContinue:
		break;
	case EAppMsgType::YesNoYesAllNoAll:
		break;
	case EAppMsgType::YesNoYesAllNoAllCancel:
		break;
	case EAppMsgType::YesNoYesAll:
		break;
	default:
		check(0);
	}

	if (ButtonWidget.IsValid())
	{
		ButtonWidget->SetParentWindow(DialogWindow.ToSharedRef());
	}
}

EAppReturnType::Type SNewMessageDialog::Open(EAppMsgType::Type MessageType, const FText& Message, const FText* OptTitle)
{
	int StrWidth = 0;
	for (const TCHAR& Ch : Message.ToString())
	{
		// 检查字符是否在常见中文字符的Unicode范围内
		if ((Ch >= 0x4E00 && Ch <= 0x9FFF) ||  // 基本汉字
			(Ch >= 0x3400 && Ch <= 0x4DBF) ||  // 扩展A区
			(Ch >= 0x20000 && Ch <= 0x2A6DF) ||  // 扩展B区
			(Ch >= 0x2A700 && Ch <= 0x2B73F) ||  // 扩展C区
			(Ch >= 0x2B740 && Ch <= 0x2B81F) ||  // 扩展D区
			(Ch >= 0x2B820 && Ch <= 0x2CEAF) ||  // 扩展E区
			(Ch >= 0xF900 && Ch <= 0xFAFF) ||  // 兼容汉字
			(Ch >= 0x2F800 && Ch <= 0x2FA1F))   // 兼容扩展
		{
			StrWidth += CHINESE_CHAR_WIDTH;
		}
		else
		{
			StrWidth += ALPHA_CHAR_WIDTH;
		}
	}

	const float Width = 200 + StrWidth;
	const float Height = 150;
	TSharedPtr<SWindow> DialogWindow = SNew(SWindow)
		.ClientSize(FVector2f(Width, Height))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		.Title(OptTitle ? *OptTitle : LOCTEXT("Title_Text", "Message"));

	TSharedRef<SNewMessageDialog> NewMessageDialog = SNew(SNewMessageDialog, DialogWindow, MessageType)
		.MessageText(Message);
	DialogWindow->SetContent(NewMessageDialog);
	FSlateApplication::Get().AddModalWindow(DialogWindow.ToSharedRef(), nullptr);

	UE_LOG(LogTemp, Display, TEXT("[NewMessageDialog]: %s"), LexToString(NewMessageDialog->GetMsgReturnType()));
	
	return NewMessageDialog->GetMsgReturnType();
}

EAppReturnType::Type SNewMessageDialog::GetMsgReturnType() const
{
	if (ButtonWidget.IsValid())
	{
		return ButtonWidget->GetMsgReturnType();
	}

	return EAppReturnType::No;
}

#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
