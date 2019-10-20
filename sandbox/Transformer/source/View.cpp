// This source file was auto-generated by ClassMate++
// Created: 18 Aug 2019 6:10:35 am
// Copyright (c) 2019, HurleyWorks

#include "jahley/window/platform/opengl/NanoguiLayer.h"
#include "View.h"

using namespace nanogui;
using juce::String;
using juce::File;
using std::cout;
using std::endl;

// ctor
View::View(const PropertyService& properties)
	: properties(properties)
{
}

// dtor
View::~View()
{
}

void View::create(NanoguiLayer* const gui, const std::string& appName)
{
	commonFolder = properties.renderProps->getVal<std::string>(RenderKey::CommonFolder);

	this->gui = gui;
	auto ctx = gui->nvgContext();
	Window* window = new Window(gui, appName);
	window->setPosition(Vector2i(15, 15));
	window->setLayout(new GroupLayout());

	modelIcons = loadImageDirectory(ctx, commonFolder + "/models");

	Label* label = new Label(window, "Background color:", "sans-bold");
	label->setColor(Color(r1, g1, b1, a1));
	ColorPicker* cp = new ColorPicker(window, { 51, 56, 61, 255 });
	cp->setFixedSize({ 100, 20 });
	cp->setCallback([&](const Color& c) {
		Eigen::Vector4f bg(c.r(), c.g(), c.b(), c.w());
		properties.renderProps->setValue(RenderKey::BackgroundColor, bg);
		// must reset the accumulator, if present
		properties.renderProps->setValue(RenderKey::ResetAccumulator, true);
		});

	cp->setFinalCallback([&](const Color& c) {
		Eigen::Vector4f bg(c.r(), c.g(), c.b(), c.w());
		properties.renderProps->setValue(RenderKey::BackgroundColor, bg);
		// must reset the accumulator, if present
		properties.renderProps->setValue(RenderKey::ResetAccumulator, true);
		});

	label = new Label(window, "Scene:", "sans-bold");
	label->setColor(Color(r1, g1, b1, a1));

	Button* b = new Button(window, "Add ground plane");
	b->setCallback([&] { emitGroundPlane(Vector2f(30.0f, 30.0f)); });
	b->setTooltip("Add a ground plane");

	PopupButton* imagePanelBtn = new PopupButton(window, "Models");
	imagePanelBtn->setIcon(ENTYPO_ICON_FOLDER);
	Popup* popup = imagePanelBtn->popup();
	VScrollPanel* vscroll = new VScrollPanel(popup);
	ImagePanel* imgPanel = new ImagePanel(vscroll);
	imgPanel->setImages(modelIcons);
	popup->setFixedSize(Vector2i(245, 150));


	imgPanel->setCallback([&, imagePanelBtn](int i) {
		imagePanelBtn->setPushed(false);
		std::string path = modelIcons[i].second;
		path = path + ".png";
		File f(path);
		if (f.existsAsFile())
		{
			emitModelPath(path);
		}
		});

	PopupButton* instanceOps = new PopupButton(window, "Instancing");
	
	Popup* instancePopup = instanceOps->popup();
	instancePopup->setLayout(new GroupLayout());

	Label * instLable = new Label(instancePopup, "Instance count :", "sans-bold");
	instLable->setColor(Color(r1, g1, b1, a1));
	auto intBox = new IntBox<int>(instancePopup);
	intBox->setCallback([&](int value) {
		properties.worldProps->setValue(WorldKey::InstanceCount, static_cast<uint32_t>(value));
		});
	intBox->setEditable(true);
	intBox->setFixedSize(Vector2i(100, 20));
	intBox->setValue(1000);
	intBox->setUnits("inst");
	intBox->setDefaultValue("0");
	intBox->setFontSize(16);
	intBox->setFormat("[1-9][0-9]*");
	intBox->setSpinnable(true);
	intBox->setMinValue(1);
	intBox->setValueIncrement(100);

	b = new Button(instancePopup, "Create instances");
	b->setCallback([&] { emitInstances(properties.worldProps->getVal<uint32_t>(WorldKey::InstanceCount)); });
	b->setTooltip("Create instances");

	b = new Button(window, "Clear scene");
	b->setCallback([&] { emitClearScene(true); });
	b->setTooltip("Clear scene");

	Label* motionLabel = new Label(window, "Motion", "sans-bold");
	motionLabel->setColor(Color(r1, g1, b1, a1));
	ComboBox * motionOps = new ComboBox(window, { "None", "Random jitter", "Reset" });
	motionOps->setCallback([&](int value) {
		properties.worldProps->setValue(WorldKey::MotionType, MotionType(value));
		});
	
	label = new Label(window, "Render:", "sans-bold");
	label->setColor(Color(r1, g1, b1, a1));

	b = new Button(window, "Frame grab");
	b->setCallback([&] { emitFrameGrab(); });
	b->setTooltip("Capture render");

	b = new Button(window, "Screen grab");
	b->setCallback([&] { emitScreenGrab(); });
	b->setTooltip("Capture screen");


	Button* about = new Button(window->buttonPanel(), "", ENTYPO_ICON_INFO);
	about->setCallback([=]() {
		std::string msg = getSceneInfo().joinIntoString("\n").toStdString();
		auto dlg = new MessageDialog(
			gui, MessageDialog::Type::Information, "Scene Info", msg);
		dlg->messageLabel()->setFixedWidth(550);
		dlg->messageLabel()->setFontSize(20);
		gui->performLayout();
		dlg->center();
		});

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