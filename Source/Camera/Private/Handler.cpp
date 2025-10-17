// Fill out your copyright notice in the Description page of Project Settings.


#include "Handler.h"

#include "ARBlueprintLibrary.h"
#include "ARTextures.h"
#include "ExternalTexture.h"                       // IExternalTextureRegistry
#include "Kismet/KismetRenderingLibrary.h"
#include "RHI.h"
#include "RHIResources.h"
#include "RHICommandList.h"

#include "ImageUtils.h"

// Sets default values
AHandler::AHandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHandler::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    UARTexture* ARCamTex = UARBlueprintLibrary::GetARTexture(EARTextureType::CameraImage);
    if (ARCamTex)
    {
        GetARTextureData(ARCamTex);
    }

}

void AHandler::GetARTextureData(UARTexture* CameraTexture)
{
    // from gemini after searching  rhi ENQUEUE_RENDER_COMMAND to read android camera data
    if (!CameraTexture || !CameraTexture->GetResource())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid Camera Texture!"));
        return;
    }

    FTextureRHIRef CameraTextureRHI = CameraTexture->GetResource()->TextureRHI;

    ENQUEUE_RENDER_COMMAND(ReadCameraData)(
        [CameraTextureRHI](FRHICommandListImmediate& RHICmdList)
        {
            // Lock the texture to read data
            FIntRect Rect(0, 0, CameraTextureRHI->GetDesc().GetSize().X, CameraTextureRHI->GetDesc().GetSize().Y);
            uint32 Stride = 0;
            void* TextureData = RHICmdList.LockTexture2D(CameraTextureRHI, 0, EResourceLockMode::RLM_ReadOnly, Stride, false);

            if (TextureData)
            {
                // Process the texture data here (e.g., copy, analyze, etc.)
                // Example: Log the first pixel value (assuming 8-bit RGBA format)
                uint8* PixelData = static_cast<uint8*>(TextureData);
                UE_LOG(LogTemp, Log, TEXT("First Pixel: R=%d, G=%d, B=%d, A=%d"),
                    PixelData[0], PixelData[1], PixelData[2], PixelData[3]);

                // Unlock the texture after processing
                RHICmdList.UnlockTexture2D(CameraTextureRHI, 0, false);
            }
        });
}