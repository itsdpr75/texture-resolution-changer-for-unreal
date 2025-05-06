#include "TextureResolutionChangerModule.h"
#include "TextureResolutionChanger.h"

#define LOCTEXT_NAMESPACE "FTextureResolutionChangerModule"

void FTextureResolutionChangerModule::StartupModule()
{
    // Este método se llama cuando el módulo se carga en memoria
}

void FTextureResolutionChangerModule::ShutdownModule()
{
    // Este método se llama cuando el módulo se descarga de la memoria
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTextureResolutionChangerModule, TextureResolutionChanger)