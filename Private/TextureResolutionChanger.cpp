#include "TextureResolutionChanger.h"
#include "LevelEditor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "EditorStyleSet.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"

#define LOCTEXT_NAMESPACE "FTextureResolutionChangerModule"

void FTextureResolutionChangerCommands::RegisterCommands()
{
    UI_COMMAND(ChangeResolution, "Change Texture Resolution", "Change the resolution of selected textures", EUserInterfaceActionType::Button, FInputChord());
}

void UTextureResolutionChanger::Initialize(FSubsystemCollectionBase& Collection)
{
    FTextureResolutionChangerCommands::Register();
    PluginCommands = MakeShareable(new FUICommandList);
    PluginCommands->MapAction(
        FTextureResolutionChangerCommands::Get().ChangeResolution,
        FExecuteAction::CreateRaw(this, &UTextureResolutionChanger::OnChangeResolutionButtonClicked),
        FCanExecuteAction());

    RegisterMenus();
}

void UTextureResolutionChanger::Deinitialize()
{
    FTextureResolutionChangerCommands::Unregister();
}

void UTextureResolutionChanger::RegisterMenus()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
    
    TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
    ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, 
        FToolBarExtensionDelegate::CreateRaw(this, &UTextureResolutionChanger::AddToolbarButton));
    
    LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
}

void UTextureResolutionChanger::AddToolbarButton(FToolBarBuilder& Builder)
{
    Builder.AddToolBarButton(FTextureResolutionChangerCommands::Get().ChangeResolution);
}

void UTextureResolutionChanger::OnChangeResolutionButtonClicked()
{
    FMenuBuilder MenuBuilder(true, PluginCommands);
    MenuBuilder.AddWidget(GenerateResolutionMenu(), FText::GetEmpty());
    
    FSlateApplication::Get().PushMenu(
        FSlateApplication::Get().GetActiveTopLevelWindow().ToSharedRef(),
        FWidgetPath(),
        MenuBuilder.MakeWidget(),
        FSlateApplication::Get().GetCursorPos(),
        FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu)
    );
}

void UTextureResolutionChanger::ChangeTextureResolution(UTexture* Texture, int32 NewResolution)
{
    if (Texture)
    {
        Texture->MaxTextureSize = NewResolution;
        Texture->UpdateResource();
        AddRecentResolution(NewResolution);
    }
}

void UTextureResolutionChanger::ResetTextureResolution(UTexture* Texture)
{
    if (Texture)
    {
        // Aquí deberías implementar la lógica para restaurar la resolución original
        // Como ejemplo, vamos a establecer un valor predeterminado de 1024
        Texture->MaxTextureSize = 1024;
        Texture->UpdateResource();
    }
}

void UTextureResolutionChanger::AddRecentResolution(int32 Resolution)
{
    RecentResolutions.RemoveSwap(Resolution);
    RecentResolutions.Insert(Resolution, 0);
    if (RecentResolutions.Num() > 5)
    {
        RecentResolutions.Pop();
    }
}

TSharedRef<SWidget> UTextureResolutionChanger::GenerateResolutionMenu()
{
    TSharedPtr<SSpinBox<int32>> ResolutionInput;
    
    return SNew(SVerticalBox)
    + SVerticalBox::Slot()
    .AutoHeight()
    .Padding(5)
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SAssignNew(ResolutionInput, SSpinBox<int32>)
            .MinValue(1)
            .MaxValue(8192)
            .Value(1024)
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(5, 0, 0, 0)
        [
            SNew(SButton)
            .Text(LOCTEXT("Apply", "Apply"))
            .OnClicked_Lambda([this, ResolutionInput]()
            {
                TArray<FAssetData> SelectedAssets;
                FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
                ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);

                for (const FAssetData& AssetData : SelectedAssets)
                {
                    if (UTexture* Texture = Cast<UTexture>(AssetData.GetAsset()))
                    {
                        ChangeTextureResolution(Texture, ResolutionInput->GetValue());
                    }
                }
                return FReply::Handled();
            })
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(5, 0, 0, 0)
        [
            SNew(SButton)
            .Text(LOCTEXT("Reset", "Reset"))
            .OnClicked_Lambda([this]()
            {
                TArray<FAssetData> SelectedAssets;
                FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
                ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);

                for (const FAssetData& AssetData : SelectedAssets)
                {
                    if (UTexture* Texture = Cast<UTexture>(AssetData.GetAsset()))
                    {
                        ResetTextureResolution(Texture);
                    }
                }
                return FReply::Handled();
            })
        ]
    ]
    + SVerticalBox::Slot()
    .AutoHeight()
    .Padding(5)
    [
        SNew(STextBlock)
        .Text(LOCTEXT("RecentResolutions", "Recent Resolutions:"))
    ]
    + SVerticalBox::Slot()
    .AutoHeight()
    .Padding(5)
    [
        SNew(SVerticalBox)
        .Visibility_Lambda([this]() { return RecentResolutions.Num() > 0 ? EVisibility::Visible : EVisibility::Collapsed; })
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SWrapBox)
            .UseAllottedWidth(true)
        ]
    ];
}

#undef LOCTEXT_NAMESPACE