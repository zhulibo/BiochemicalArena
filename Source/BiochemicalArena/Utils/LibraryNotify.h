#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LibraryNotify.generated.h"

#define NOTIFY(Context, DisplayColor, Msg) ULibraryNotify::ShowNotify(Context, DisplayColor, Msg)

UCLASS()
class BIOCHEMICALARENA_API ULibraryNotify : public UObject
{
	GENERATED_BODY()

public:
	static void ShowNotify(const UObject* Context, FColor DisplayColor, const FText& Msg);

};
