module;
#include <imgui/imgui/imgui.h>

export module imguiext;

void checkBounds(auto* v, auto minValue, auto maxValue)
{
	if (*v < minValue)
		*v = minValue;

	if (*v > maxValue)
		*v = maxValue;
}

export namespace ext
{
	bool InputClampedInt(const char* label, int* v, int minValue, int maxValue)
	{
		checkBounds(v, minValue, maxValue);

		return ImGui::InputInt(label, v);
	}

	bool InputClampedFloat(const char* label, float* v, float minValue, float maxValue)
	{
		checkBounds(v, minValue, maxValue);

		return ImGui::InputFloat(label, v);;
	}

	bool InputClampedDouble(const char* label, double* v, double minValue, double maxValue)
	{
		checkBounds(v, minValue, maxValue);

		return ImGui::InputDouble(label, v);
	}
}