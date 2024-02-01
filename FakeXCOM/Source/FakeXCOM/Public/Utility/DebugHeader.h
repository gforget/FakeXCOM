// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once
inline void DebugScreen(const FString& Message, const FColor& Color)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.0f, Color, Message);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
	}
}

