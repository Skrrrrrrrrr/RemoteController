#ifndef BLINK_H
#define BLINK_H
#include <stdint.h>
class Blink{
	public:
		static Blink &instance();
		void toggle();
	private:
		explicit Blink();
	public:
		uint32_t count;
};
#endif