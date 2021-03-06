// This header file was auto-generated by ClassMate++
// Created: 27 Oct 2012 6:06:34 am
// Copyright (c) 2012, HurleyWorks

#pragma  once

using wabi::Ray3f;

class InputEvent
{

 public:
	enum Type
	{
	  Idle = 0,
	  Move,
	  Press,
	  Release,
	  ScrollDown,
	  ScrollUp,
	  DblClick,
	  Drag
	};

	enum MouseButton
	{
	  Left,
	  Middle,
	  Right,
	  VScroll 
	};

	enum Modifier
	{
		Alt = 1,
		Ctrl = 2,
		Shift = 4
	};

 public:
	InputEvent ();
	InputEvent (const Type & type,
                const MouseButton & button, 
                float x, 
                float y, 
                bool alt = false, 
                bool ctrl = false, 
                bool shift = false);
	~InputEvent ();

	 const MouseButton & getButton () const { return button_; }
	 unsigned int getKeyboardModifiers () const { return keyState_; }
	 const Type & getType () const { return type_; }
	 float getX () const { return mouseX_; }
	 float getY () const { return mouseY_; }
	 void getScreenMovement(float & deltaX, float & deltaY) { deltaX = dx_; deltaY = dy_; }
	 float getScreenMovementX() const { return dx_; }
	 float getScreenMovementY() const { return dy_; }
		
	 void setX(float x) { mouseX_ = x; }
	 void setY(float y) { mouseY_ = y; }
	 void setButton (const MouseButton & button) { button_ = button; }
	 void setType (const Type & type) { type_ = type; }
	 void setScreenMovement( float deltaX, float deltaY ) { dx_ = deltaX; dy_ = deltaY; }
	 void setKeyboardModifiers(unsigned int state) { keyState_ = state; }

	 void setPickRay(const Ray3f& ray) { pickRay = ray; }
	 const Ray3f& getPickRay() const { return pickRay; }
	 Ray3f& getPickRay() { return pickRay; }

 private:
	 Type type_ = Type::Idle;
	 Type prevType_ = Type::Idle;
	 MouseButton button_ = MouseButton::Left;
	 float mouseX_ = 0.0f;
	 float mouseY_ = 0.0f;
	 unsigned int keyState_ = 0;
	 float dx_ = 0.0f;
	 float dy_ = 0.0f;
	 Ray3f pickRay;
	 
}; // end class InputEvent