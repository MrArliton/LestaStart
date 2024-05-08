#pragma once

#include "CoreMinimal.h"
#include "LestaTypes.generated.h"

/** Teams */
UENUM(BlueprintType)
enum class ETeam : uint8
{
    TEAM_NONE UMETA(DisplayName = "NONE"),
    TEAM_PLAYERS UMETA(DisplayName = "PLAYERS"),
    TEAM_AI UMETA(DisplayName = "AI"),
};