#pragma once

UENUM(BlueprintType)
enum class ECurveValueType : uint8
{
	ECVT_None			UMETA(DisplayName = "None"),
	ECVT_Scalar			UMETA(DisplayName = "ScalarValue"),
	ECVT_LinearColor	UMETA(DisplayNAme = "LinearColor")
};