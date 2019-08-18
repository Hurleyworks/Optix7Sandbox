
#pragma once

// from https://github.com/memononen/nanovg zlib 

#include <nanovg.h>
#include <nanogui/nanogui.h>

enum GraphrenderStyle {
    GRAPH_RENDER_FPS,
    GRAPH_RENDER_MS,
    GRAPH_RENDER_PERCENT,
};

#define GRAPH_HISTORY_COUNT 100
struct PerfGraph {
	int style;
	char name[32];
	float values[GRAPH_HISTORY_COUNT];
	int head;
};
typedef struct PerfGraph PerfGraph;

void initGraph(PerfGraph* fps, int style, const char* name);
void updateGraph(PerfGraph* fps, float frameTime);
void renderGraph(NVGcontext* vg, float x, float y, PerfGraph* fps);
float getGraphAverage(PerfGraph* fps);

#define GPU_QUERY_COUNT 5
struct GPUtimer {
	int supported;
	int cur, ret;
	unsigned int queries[GPU_QUERY_COUNT];
};
typedef struct GPUtimer GPUtimer;

void initGPUTimer(GPUtimer* timer);
void startGPUTimer(GPUtimer* timer);
int stopGPUTimer(GPUtimer* timer, float* times, int maxTimes);
