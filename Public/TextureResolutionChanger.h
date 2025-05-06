#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "Framework/Commands/Commands.h"
#include "TextureResolutionChanger.generated.h"

class FTextureResolutionChangerCommands : public TCommands<FTextureResolutionChangerCommands>
{
public:
    FTextureResolutionChangerCommands()
        : TCommands<FTextureResolutionChangerCommands>(
              TEXT("TextureResolutionChanger"),
              NSLOCTEXT("Contexts", "TextureResolutionChanger", "Texture Resolution Changer Plugin"),
              NAME_None,
              FEditorStyle::GetStyleSetName())
    {
    }

    virtual void RegisterCommands() override;

    TSharedPtr<FUICommandInfo> ChangeResolution;
};

UCLASS()
class TEXTURERESOLUTIONCHANGER_API UTextureResolutionChanger : public UEditorSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

private:
    TSharedPtr<FUICommandList> PluginCommands;
    TArray<int32> RecentResolutions;
    
    void RegisterMenus();
    void AddToolbarButton(FToolBarBuilder& Builder);
    void OnChangeResolutionButtonClicked();
    void ChangeTextureResolution(UTexture* Texture, int32 NewResolution);
    void ResetTextureResolution(UTexture* Texture);
    void AddRecentResolution(int32 Resolution);
    TSharedRef<SWidget> GenerateResolutionMenu();
};