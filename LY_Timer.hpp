namespace LY{
	namespace Timer{
			void Setup();
			void Phase(int hz);
			void Wait(int ticks);
			void Destroy(int id);
			void Create(int id, int expire);
		}
}
extern "C"
void LY_Timer_Handler(struct regs* r);
