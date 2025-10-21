#pragma once

#include <string>
#include <optional>

namespace Lexvi {
	template<typename T>
	class Setting {
	private:
		T value;

	public:
		Setting() = default;
		Setting(const T& val) : value(val) {};

		~Setting() = default;

	public:
		const T& getValue() const {
			return value;
		}

		void setValue(const T& value) {
			this->value = value;
		}

	public:
		void operator= (const T& val) {
			value = val;
		}
	};

	template<typename T>
	class OptionalSetting
	{
	private:
		std::optional<bool> enabled;
		std::optional<T> value;
		std::string description = "";

	public:
		OptionalSetting() = default;
		~OptionalSetting() = default;

	public:
		bool hasEnabled() const {
			return enabled.has_value();
		}
		bool hasValue() const {
			return value.has_value();
		}
		bool isBool() const {
			return enabled.has_value() && !value.has_value();
		}

		const T& getValue() const {
			value.has_value() ? value.value() : T();
		}

	public:
		void setValue(const T& value) {
			this->value = value;
		}
		void setEnabled(bool enabled) {
			this->enabled = enabled;
		}
	};
}