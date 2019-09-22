// This source file was auto-generated by ClassMate++
// Created: 18 Aug 2019 6:10:35 am
// Copyright (c) 2019, HurleyWorks

#include "jahley/window/platform/opengl/NanoguiLayer.h"
#include "View.h"

using namespace nanogui;
using juce::String;
using std::cout;
using std::endl;

// ctor
View::View (const PropertyService& properties)
	: properties(properties)
{	
}

// dtor
View::~View ()
{	
}

void View::create(NanoguiLayer* const gui, const std::string& appName)
{
	this->gui = gui;

	Window* window = new Window(gui, appName);
	window->setPosition(Vector2i(15, 15));
	window->setLayout(new GroupLayout());

	Label * label = new Label(window, "Background color:", "sans-bold");
	label->setColor(Color(r1, g1, b1, a1));
	ColorPicker * cp = new ColorPicker(window, { 51, 56, 61, 255 });
	cp->setFixedSize({ 100, 20 });
	cp->setCallback([&](const Color& c) {
		Eigen::Vector4f bg(c.r(), c.g(), c.b(), c.w());
		properties.renderProps->setValue(RenderKey::BackgroundColor, bg);
		});

	cp->setFinalCallback([&](const Color& c) {
		Eigen::Vector4f bg(c.r(), c.g(), c.b(), c.w());
		properties.renderProps->setValue(RenderKey::BackgroundColor, bg);
		});

	label = new Label(window, "Mesh color:", "sans-bold");
	label->setColor(Color(r1, g1, b1, a1));
	Vector4f col = properties.renderProps->getVal<Vector4f>(RenderKey::MeshColor);
	
	cp = new ColorPicker(window, nanogui::Color(col.x(), col.y(), col.z(), 1.0f));
	cp->setFixedSize({ 100, 20 });
	cp->setCallback([&](const Color& c) {
		Eigen::Vector4f bg(c.r(), c.g(), c.b(), c.w());
		properties.renderProps->setValue(RenderKey::MeshColor, bg);
		});

	cp->setFinalCallback([&](const Color& c) {
		Eigen::Vector4f bg(c.r(), c.g(), c.b(), c.w());
		properties.renderProps->setValue(RenderKey::MeshColor, bg);
		});

	Button *b = new Button(window, "Screen grab");
	b->setCallback([&] { emitScreenGrab(); });
	b->setTooltip("Capture screen");

	gui->performLayout();
}

const StringArray View::getSceneInfo()
{
	StringArray message;

	uint64_t totalMeshes = properties.worldProps->getVal<uint64_t>(WorldKey::TotalMeshes);
	uint64_t totalInstances = properties.worldProps->getVal<uint64_t>(WorldKey::TotalInstances);
	uint64_t totalInstancedTriangles = properties.worldProps->getVal<uint64_t>(WorldKey::TotalInstancedTriangles);
	uint64_t totalTriangles = properties.worldProps->getVal<uint64_t>(WorldKey::TotalRealTriangles);

	message.add(String("Total meshes: ") + String(totalMeshes));
	message.add(String("Total instances: ") + String(totalInstances));
	message.add(String("Total mesh triangles: ") + String(totalTriangles));
	message.add(String("Total instanced triangles: ") + String(totalInstancedTriangles));

	return message;
}