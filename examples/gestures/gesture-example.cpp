/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <string>
#include <dali/devel-api/events/rotation-gesture.h>
#include <dali/devel-api/events/rotation-gesture-detector.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace std;

namespace
{
const Property::Value BACKGROUND
{
  { Toolkit::Visual::Property::TYPE, Visual::GRADIENT },
  { GradientVisual::Property::STOP_COLOR,  Property::Array{ Vector4( 167.0f, 207.0f, 223.0f, 255.0f ) / 255.0f,
                                                            Vector4(   0.0f,  64.0f, 137.0f, 255.0f ) / 255.0f } },
  { GradientVisual::Property::START_POSITION, Vector2( 0.0f, -0.5f ) },
  { GradientVisual::Property::END_POSITION,   Vector2( 0.0f,  0.5f ) }
};

const Property::Value CONTROL_BACKGROUND
{
  { Toolkit::Visual::Property::TYPE, Visual::GRADIENT },
  { GradientVisual::Property::STOP_COLOR, Property::Array{ Vector4( 234.0f, 185.0f,  45.0f, 255.0f ) / 255.0f,
                                                           Vector4( 199.0f, 152.0f,  16.0f, 255.0f ) / 255.0f } },
  { GradientVisual::Property::CENTER, Vector2::ZERO },
  { GradientVisual::Property::RADIUS, 0.5f }
};

const float HELP_ANIMATION_DURATION( 25.0f );
const float HELP_ANIMATION_SEGMENT_TIME( 5.0f );
const float HELP_ANIMATION_TRANSITION_DURATION( 0.75f );
const Vector2 HELP_TEXT_POSITION_MULTIPLIER( 0.25f, 0.13f );

const float SHAKY_ANIMATION_DURATION( 0.1f );
const float SHAKY_ANIMATION_SEGMENT_TIME( 0.05f );
const float SHAKY_ANIMATION_ANGLE( 1.0f );

const float TOUCH_MODE_ANIMATION_DURATION( 0.1f );
const Vector4 TOUCH_MODE_COLOR( 1.0f, 0.7f, 0.7f, 1.0f );

const float PAN_MODE_CHANGE_ANIMATION_DURATION( 0.25f );
const Vector3 PAN_MODE_START_ANIMATION_SCALE( 1.2f, 1.2f, 1.0f );
const Vector3 PAN_MODE_END_ANIMATION_SCALE( 0.8f, 0.8f, 1.0f );

const float TAP_ANIMATION_DURATION( 0.5f );
const Vector4 TAP_ANIMATION_COLOR( 0.8f, 0.5, 0.2f, 0.6f );

const Vector3 MINIMUM_SCALE( Vector3::ONE );
const Vector3 MAXIMUM_SCALE( Vector3::ONE * 2.0f );
const float SCALE_BACK_ANIMATION_DURATION( 0.25f );
const float ROTATE_BACK_ANIMATION_DURATION( 0.25f );

/**
 * @brief Shows the given string between the given start and end times.
 *
 * Appropriately animates the string into and out of the scene.
 *
 * @param[in]  string     The label to display, this is an rvalue reference & will be moved
 * @param[in]  parent     The parent to add the label to
 * @param[in]  animation  The animation to add the animators created in this function
 * @param[in]  startTime  When to start the animators
 * @param[in]  endTime    When to end the animators
 */
void AddHelpInfo( const std::string&& string, Actor parent, Animation animation, float startTime, float endTime )
{
  Actor text = TextLabel::New( std::move( string ) );
  Vector3 position( Stage::GetCurrent().GetSize() * HELP_TEXT_POSITION_MULTIPLIER );

  text.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  text.SetParentOrigin( ParentOrigin::TOP_CENTER );
  text.SetPosition( position );
  text.SetOpacity( 0.0f );
  text.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, Text::HorizontalAlignment::CENTER );
  text.SetProperty( TextLabel::Property::MULTI_LINE, true );
  parent.Add( text );

  // Animate IN
  TimePeriod timePeriod( startTime, HELP_ANIMATION_TRANSITION_DURATION );
  animation.AnimateTo( Property( text, Actor::Property::COLOR_ALPHA ),  1.0f,       timePeriod );
  animation.AnimateBy( Property( text, Actor::Property::POSITION_X ),  -position.x, timePeriod );

  // Animate OUT
  timePeriod.delaySeconds = endTime;
  animation.AnimateTo( Property( text, Actor::Property::COLOR_ALPHA ),  0.0f,       timePeriod );
  animation.AnimateBy( Property( text, Actor::Property::POSITION_X ),  -position.x, timePeriod );
}

} // unnamed namespace

/**
 * @brief This example shows how to use the different gesture detectors available.
 *
 * - Tapping on the control shows a small rotation animation.
 * - A Long press on the control puts it into Pan Mode.
 * - When in Pan mode, the control can be panned (moved).
 * - When the pan ends, we exit the Pan mode via an animation.
 * - Pinching the control changes the scale of the control.
 */
class GestureExample : public ConnectionTracker
{
public:

  /**
   * @brief Constructor.
   *
   * @param[in]  application  Reference to the application
   */
  GestureExample( Application &application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    application.InitSignal().Connect( this, &GestureExample::Create );
  }

private:

  /**
   * @brief Creates the scene as described in this class' description.
   *
   * @param[in]  application  Reference to the application class
   */
  void Create( Application& application )
  {
    // Get a handle to the stage & connect to the key event signal
    Stage stage = Stage::GetCurrent();
    stage.KeyEventSignal().Connect(this, &GestureExample::OnKeyEvent);

    // Create a background with a linear gradient which matches parent size & is placed in the center.
    Actor background = Control::New();
    background.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    background.SetParentOrigin( ParentOrigin::CENTER );
    background.SetProperty( Control::Property::BACKGROUND, BACKGROUND );
    stage.Add( background );

    // Create a control with a circular gradient that we'll use for the gestures and be a quarter of the size of the stage.
    Actor touchControl = Control::New();
    touchControl.SetSize( stage.GetSize() * 0.25f );
    touchControl.SetParentOrigin( ParentOrigin::CENTER );
    touchControl.SetProperty( Control::Property::BACKGROUND, CONTROL_BACKGROUND );
    background.Add( touchControl );

    // Connect to the touch signal
    touchControl.TouchSignal().Connect( this, &GestureExample::OnTouch );
    touchControl.SetLeaveRequired( true );

    // Create a long press gesture detector, attach the actor & connect
    mLongPressDetector = LongPressGestureDetector::New();
    mLongPressDetector.Attach( touchControl );
    mLongPressDetector.DetectedSignal().Connect( this, &GestureExample::OnLongPress );

    // Create a pan gesture detector & connect, don't attach the actor as we'll attach it when we detect a long-press
    mPanDetector = PanGestureDetector::New();
    mPanDetector.DetectedSignal().Connect( this, &GestureExample::OnPan );

    // Create a tap gesture detector, attach the actor & connect
    mTapDetector = TapGestureDetector::New();
    mTapDetector.Attach( touchControl );
    mTapDetector.DetectedSignal().Connect( this, &GestureExample::OnTap );

    // Create a pinch gesture detector, attach the actor & connect
    mPinchDetector = PinchGestureDetector::New();
    mPinchDetector.Attach( touchControl );
    mPinchDetector.DetectedSignal().Connect( this, &GestureExample::OnPinch );

    // Create a rotation gesture detector, attach the actor & connect
    mRotationDetector = RotationGestureDetector::New();
    mRotationDetector.Attach( touchControl );
    mRotationDetector.DetectedSignal().Connect( this, &GestureExample::OnRotation );

    // Create an animation which shakes the actor when in Pan mode
    mShakeAnimation = Animation::New( SHAKY_ANIMATION_DURATION );
    mShakeAnimation.AnimateBy( Property( touchControl, Actor::Property::ORIENTATION ),
                               Quaternion( Degree(  SHAKY_ANIMATION_ANGLE ), Vector3::ZAXIS ),
                               AlphaFunction::BOUNCE,
                               TimePeriod( 0.0f, SHAKY_ANIMATION_SEGMENT_TIME ) );
    mShakeAnimation.AnimateBy( Property( touchControl, Actor::Property::ORIENTATION ),
                               Quaternion( Degree( -SHAKY_ANIMATION_ANGLE ), Vector3::ZAXIS ),
                               AlphaFunction::BOUNCE,
                               TimePeriod( SHAKY_ANIMATION_SEGMENT_TIME, SHAKY_ANIMATION_SEGMENT_TIME ) );

    // Animate help information
    // Here we just animate some text on the screen to show tips on how to use this example
    // The help animation loops
    Animation helpAnimation = Animation::New( HELP_ANIMATION_DURATION );

    float startTime( 0.0f );
    float endTime( startTime + HELP_ANIMATION_SEGMENT_TIME );

    AddHelpInfo( "Tap image for animation",                              background, helpAnimation, startTime, endTime );
    AddHelpInfo( "Press & Hold image to drag",                           background, helpAnimation, startTime += HELP_ANIMATION_SEGMENT_TIME, endTime += HELP_ANIMATION_SEGMENT_TIME );
    AddHelpInfo( "Pinch image to resize",                                background, helpAnimation, startTime += HELP_ANIMATION_SEGMENT_TIME, endTime += HELP_ANIMATION_SEGMENT_TIME );
    AddHelpInfo( "Move fingers in a circular motion on image to rotate", background, helpAnimation, startTime += HELP_ANIMATION_SEGMENT_TIME, endTime += HELP_ANIMATION_SEGMENT_TIME );
    helpAnimation.SetLooping( true );
    helpAnimation.Play();
  }

  /**
   * @brief Called when our actor is touched.
   *
   * @param[in]  actor  The touched actor
   * @param[in]  touch  The touch event
   */
  bool OnTouch( Actor actor, const TouchData& touch )
  {
    switch( touch.GetState( 0 ) )
    {
      case PointState::DOWN:
      {
        // When we get a touch point, change the color of the actor.

        Animation anim = Animation::New( TOUCH_MODE_ANIMATION_DURATION );
        anim.AnimateTo( Property( actor, Actor::Property::COLOR ), TOUCH_MODE_COLOR );
        anim.Play();
        break;
      }

      case PointState::LEAVE:
      case PointState::UP:
      case PointState::INTERRUPTED:
      {
        if( ! mPanStarted )
        {
          // If we're not panning, change the color back to normal.

          Animation anim = Animation::New( TOUCH_MODE_ANIMATION_DURATION );
          anim.AnimateTo( Property( actor, Actor::Property::COLOR ), Vector4::ONE );
          anim.Play();

          // Stop the shake animation from looping.
          mShakeAnimation.SetLooping( false );
        }
        break;
      }

      default:
      {
        break;
      }
    }
    return true;
  }

  /**
   * @brief Called when a long-press gesture is detected on our control.
   *
   * @param[in]  actor      The actor that's been long-pressed
   * @param[in]  longPress  The long-press gesture information
   */
  void OnLongPress( Actor actor, const LongPressGesture& longPress )
  {
    if( longPress.state == Gesture::Started )
    {
      // When we first receive a long press, attach the actor to the pan detector.
      mPanDetector.Attach( actor );

      // Do a small animation to indicate to the user that we are in pan mode.
      Animation anim = Animation::New( PAN_MODE_CHANGE_ANIMATION_DURATION );
      anim.AnimateTo( Property( actor, Actor::Property::SCALE ), actor.GetCurrentScale() * PAN_MODE_START_ANIMATION_SCALE, AlphaFunction::BOUNCE );
      anim.Play();

      // Start the shake animation so the user knows when they are in pan mode.
      mShakeAnimation.SetLooping( true );
      mShakeAnimation.Play();
    }
  }

  /**
   * @brief Called when a pan gesture is detected on our control.
   *
   * @param[in]  actor  The actor that's been panned
   * @param[in]  pan    The pan gesture information
   */
  void OnPan( Actor actor, const PanGesture& pan )
  {
    // Just move the actor by the displacement.

    // As the displacement is in local actor coords, we will have to multiply the displacement by the
    // actor's scale so that it moves the correct amount in the parent's coordinate system.
    Vector3 scaledDisplacement( pan.displacement );
    scaledDisplacement *= actor.GetCurrentScale();

    Vector3 currentPosition;
    actor.GetProperty( Actor::Property::POSITION ).Get( currentPosition );

    Vector3 newPosition = currentPosition + scaledDisplacement;
    actor.SetPosition( newPosition );

    switch( pan.state )
    {
      case Gesture::Started:
      {
        mPanStarted = true;
        break;
      }

      case Gesture::Finished:
      case Gesture::Cancelled:
      {
        // If we cancel or finish the pan, do an animation to indicate this and stop the shake animation.

        Animation anim = Animation::New( PAN_MODE_CHANGE_ANIMATION_DURATION );
        anim.AnimateTo( Property( actor, Actor::Property::COLOR ), Vector4::ONE );
        anim.AnimateTo( Property( actor, Actor::Property::SCALE ), actor.GetCurrentScale() * PAN_MODE_END_ANIMATION_SCALE, AlphaFunction::BOUNCE );

        // Move actor back to center if we're out of bounds
        Vector2 halfStageSize = Stage::GetCurrent().GetSize() * 0.5f;
        if( ( abs( newPosition.x ) > halfStageSize.width  ) ||
            ( abs( newPosition.y ) > halfStageSize.height ) )
        {
          anim.AnimateTo( Property( actor, Actor::Property::POSITION ), Vector3::ZERO, AlphaFunction::EASE_IN );
        }
        anim.Play();

        // Set end of pan configuration and disconnect the actor from the pan detector
        mShakeAnimation.SetLooping( false );
        mPanStarted = false;
        mPanDetector.Detach( actor );
        break;
      }

      default:
      {
        break;
      }
    }
  }

  /**
   * @brief Called when a tap gesture is detected on our control.
   *
   * @param[in]  actor The actor that's been tapped
   * @param[in]  tap   The tap gesture information
   */
  void OnTap( Actor actor, const TapGesture& tap )
  {
    // Do a short animation to show a tap has happened.

    Animation anim = Animation::New( TAP_ANIMATION_DURATION );
    anim.AnimateBy( Property( actor, Actor::Property::ORIENTATION ), Quaternion( ANGLE_360, Vector3::ZAXIS ) );
    anim.AnimateTo( Property( actor, Actor::Property::SCALE ), Vector3::ONE, AlphaFunction::LINEAR );
    anim.AnimateTo( Property( actor, Actor::Property::COLOR ), TAP_ANIMATION_COLOR, AlphaFunction::BOUNCE );
    anim.AnimateTo( Property( actor, Actor::Property::POSITION ), Vector3::ZERO, AlphaFunction::EASE_OUT_SQUARE );
    anim.Play();
  }

  /**
   * @brief Called when a pinch gesture is detected on our control.
   *
   * @param[in]  actor  The actor that's been pinched
   * @param[in]  pinch  The pinch gesture information
   */
  void OnPinch( Actor actor, const PinchGesture& pinch )
  {
    switch( pinch.state )
    {
      case Gesture::Started:
      {
        // Starting scale is required so that we know what to multiply the pinch.scale by.
        mStartingScale = actor.GetCurrentScale();
        break;
      }

      case Gesture::Finished:
      case Gesture::Cancelled:
      {
        Vector3 scale( actor.GetCurrentScale() );

        // Ensure the actor sizes itself to be within the limits defined.
        if ( scale.x < MINIMUM_SCALE.x )
        {
          scale = MINIMUM_SCALE;
        }
        else if ( scale.x > MAXIMUM_SCALE.x )
        {
          scale = MAXIMUM_SCALE;
        }

        // Do an animation to come back to go back to the limits.
        Animation anim = Animation::New( SCALE_BACK_ANIMATION_DURATION );
        anim.AnimateTo( Property( actor, Actor::Property::SCALE ), scale, AlphaFunction::LINEAR );
        anim.Play();
        break;
      }

      default:
      {
        break;
      }
    }

    actor.SetScale( mStartingScale * pinch.scale );
  }

  /**
   * @brief Called when a rotation gesture is detected on our control.
   *
   * @param[in]  actor     The actor that's been pinched
   * @param[in]  rotation  The rotation gesture information
   */
  void OnRotation( Actor actor, const RotationGesture& rotation )
  {
    switch( rotation.state )
    {
      case Gesture::Started:
      {
        // Starting orientation is required so that we know what to multiply the rotation.rotation by.
        mStartingOrientation = actor.GetCurrentOrientation();
        break;
      }

      case Gesture::Finished:
      case Gesture::Cancelled:
      {
        // Do an animation to come back to go back to the original orientation.
        Animation anim = Animation::New( ROTATE_BACK_ANIMATION_DURATION );
        anim.AnimateTo( Property( actor, Actor::Property::ORIENTATION ), Quaternion::IDENTITY, AlphaFunction::LINEAR );
        anim.Play();
        break;
      }

      default:
      {
        break;
      }
    }

    actor.SetOrientation( mStartingOrientation * Quaternion( rotation.rotation, Vector3::ZAXIS ) );
  }

  /**
   * @brief Called when any key event is received.
   *
   * Will use this to quit the application if Back or the Escape key is received.
   * @param[in] event The key event information
   */
  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application&  mApplication;

  PanGestureDetector mPanDetector;
  LongPressGestureDetector mLongPressDetector;
  TapGestureDetector mTapDetector;
  PinchGestureDetector mPinchDetector;
  RotationGestureDetector mRotationDetector;

  Vector3 mStartingScale; ///< Set to the scale of the control when pinch starts.
  Quaternion mStartingOrientation; ///< Set to the orientation of the control when the rotation starts.
  Animation mShakeAnimation; ///< "Shake" animation to show when we are in panning mode.
  bool mPanStarted = false; ///< Set to true to state that panning has started.
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  GestureExample controller( application );
  application.MainLoop();
  return 0;
}
