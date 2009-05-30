#ifndef clock_h
#define clock_h

/**
 * NES clock definition. It counts the number of cycles that have
 * elapsed since the start of the machine. The cycles are counted
 * in PPU cycles and CPU cycles.
 * As an addition, a count of the cycles elapsed since the last
 * NMI (in CPU and PPY cycles) is maintained
 *
 * The C representation of these counts is big enough to count
 * the cycles continuously without overflowing for about
 * ~50.000 years, so we don't have to take this case into account :)
 */
typedef struct _clock {

	/* CPU counter */
	unsigned long int cpu_cycles;   /* CPU cycles counter */
	unsigned int nmi_ccycles;       /* CPU cycles since the last NMI */

	/* PPU counter */
	unsigned long int ppu_cycles;  /* PPU cycles */
	unsigned int nmi_pcycles;      /* PPU cycles since the last NMI */

} nes_clock;

extern nes_clock *CLK;

/**
 * Adds a given number of CPU cycles to the clock counting
 */
#define ADD_CPU_CYCLES(N) \
	do { \
		CLK->cpu_cycles  += N; \
		CLK->nmi_ccycles += N; \
		CLK->ppu_cycles  += 3*(N); \
		CLK->nmi_pcycles += 3*(N); \
	} while (0)


/**
 * Initializes the CLK data
 */
void initialize_clock();

/**
 * Dumps the contents of the clock to stdout
 */
void dump_clock();

#endif /* clock_h */
