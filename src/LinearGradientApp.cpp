#include "cinder/app/AppBasic.h"
#include "cinder/gl/GlslProg.h"

using namespace ci;
using namespace ci::app;

class LinearGradientApp : public AppBasic {
public:
    void setup();
    void keyDown( KeyEvent event );
    void mouseDown( MouseEvent event );
    void mouseUp( MouseEvent event );
    void mouseDrag( MouseEvent event );
    
    void update();
    void draw();
    
    gl::TextureRef	mTexture;
    gl::GlslProgRef	mShader;
    float			mAngle;
    
    // Gradient
    Vec2f                   mGradientStart, mGradientEnd;
    std::vector< float >    mGradientStops;
    std::vector< Vec4f >    mGradientColors;
    
    // Interaction
    Vec2f *                 mDraggedGradientPos;
    float *                 mDraggedGradientStop;
    
};


void LinearGradientApp::setup()
{
    
    try {
        mShader = gl::GlslProg::create( loadResource( "LinearGradient_vert.glsl" ), loadResource( "LinearGradient_frag.glsl" ) );
    }
    catch( gl::GlslProgCompileExc &exc ) {
        std::cout << "Shader compile error: " << std::endl;
        std::cout << exc.what();
    }
    catch( ... ) {
        std::cout << "Unable to load shader" << std::endl;
    }
    
    // Prepare sample gradient
    mGradientStart = Vec2f( 100, 100 );
    mGradientEnd   = getWindowSize() - Vec2f( 100, 100 );
    
    mGradientStops.push_back( 0.10f );
    mGradientStops.push_back( 0.25f );
    mGradientStops.push_back( 0.75f );
    mGradientStops.push_back( 0.90f );
    
    mGradientColors.push_back( Vec4f(1.f, 0.f, 0.f, 1.f) );
    mGradientColors.push_back( Vec4f(1.f, 0.f, 1.f, 1.f) );
    mGradientColors.push_back( Vec4f(0.f, 0.f, 1.f, 1.f) );
    mGradientColors.push_back( Vec4f(0.f, 1.f, 0.f, 1.f) );
    
    // Interaction
    mDraggedGradientPos  = NULL;
    mDraggedGradientStop = NULL;
    
    
}


void LinearGradientApp::update()
{
    double t = getElapsedSeconds();
    
    mGradientColors[0].y = 0.5f + sin( t/1.0 )/2;
    mGradientColors[1].z = 0.5f + cos( t/1.0 )/2;
    mGradientColors[2].y = 0.5f + sin( t/1.0 )/2;
    mGradientColors[3].z = 0.5f + cos( t/1.0 )/2;
}

void LinearGradientApp::draw()
{
    gl::clear( Color::white() );
    
    // Draw shader gradient
    mShader->bind();
    mShader->uniform( "numStops", (int)mGradientColors.size() );
    mShader->uniform( "gradientStartPos", mGradientStart );
    mShader->uniform( "gradientEndPos", mGradientEnd );
    mShader->uniform( "colors", &mGradientColors[0], mGradientColors.size() );
    mShader->uniform( "stops", &mGradientStops[0], mGradientStops.size() );
    mShader->uniform( "windowHeight", (float)getWindowHeight() );
    
    //    gl::drawSolidRect( Rectf( Vec2f( 0, 0), getWindowSize() ) );
    gl::drawSolidRect( Rectf( getWindowCenter()-Vec2f( 200, 100), getWindowCenter() + Vec2f( 200, 100) ) );
    mShader->unbind();
    
    
    // Draw controls
    gl::color( Color::black() );
    gl::drawLine( mGradientStart, mGradientEnd );
    gl::drawSolidCircle( mGradientStart, 8 );
    gl::drawSolidCircle( mGradientEnd, 8 );
    
    float d = mGradientStart.distance( mGradientEnd );
    float alpha = atan2f( -mGradientEnd.y + mGradientStart.y, mGradientEnd.x - mGradientStart.x );
    for ( size_t i=0; i<mGradientStops.size(); ++i ) {
        gl::color( ColorA( mGradientColors[i].x, mGradientColors[i].y, mGradientColors[i].z, mGradientColors[i].w  ) );
        
        Vec2f p = mGradientStart + Vec2f( d*mGradientStops[i]*cos( alpha ), -d*mGradientStops[i]*sin( alpha ) );
        
        gl::drawSolidCircle( p, 10 );
        
        gl::lineWidth( 2 );
        gl::color( Color::black() );
        gl::drawStrokedCircle( p, 10 );
        
    }
    
}

#pragma mark -
#pragma mark Interactions
void LinearGradientApp::mouseDown( MouseEvent event ) {
    Vec2f mousePos = event.getPos();
    
    float d = mGradientStart.distance( mGradientEnd );
    float alpha = atan2f( -mGradientEnd.y + mGradientStart.y, mGradientEnd.x - mGradientStart.x );
    
    for ( float &stop : mGradientStops ) {
        if ( mousePos.distanceSquared( mGradientStart + Vec2f( stop*d*cos( alpha ), -stop*d*sin( alpha ) ) ) < 101 ) {
            mDraggedGradientStop = &stop;
            std::cout << "Gradient Stop is selected!" << std::endl;
            return;
        }
    }
    
    
    if ( mousePos.distanceSquared( mGradientStart ) < 65 ) {
        mDraggedGradientPos = &mGradientStart;
        std::cout << "Gradient Start Pos is selected!" << std::endl;
    }
    else if ( mousePos.distanceSquared( mGradientEnd ) < 65 ) {
        mDraggedGradientPos = &mGradientEnd;
        std::cout << "Gradient End Pos is selected!" << std::endl;
    }
    
}

void LinearGradientApp::mouseUp( MouseEvent event ) {
    mDraggedGradientPos  = NULL;
    mDraggedGradientStop = NULL;
}

void LinearGradientApp::mouseDrag( MouseEvent event ) {
    Vec2f mousePos = event.getPos();
    float alpha = atan2f( -mGradientEnd.y + mGradientStart.y, mGradientEnd.x - mGradientStart.x );
    
    if ( mDraggedGradientPos != NULL ) {
        //        *mDraggedGradientPos = event.getPos();
        mDraggedGradientPos->set( mousePos );
    }
    
    if ( mDraggedGradientStop != NULL ) {
        // Get a projection to Gradient direction axis
        float  gradientStartProj = mGradientStart.x*cos(alpha) - mGradientStart.y*sin(alpha);
        float  gradientEndProj   = mGradientEnd.x*cos(alpha)   - mGradientEnd.y*sin(alpha);
        float  mousePosProj      = mousePos.x*cos( alpha ) - mousePos.y*sin( alpha );
        
        float  newLoc = (mousePosProj - gradientStartProj)/(gradientEndProj - gradientStartProj);
        
        newLoc = ( newLoc < 0.f ) ? 0.f : newLoc;
        newLoc = ( newLoc > 1.f ) ? 1.f : newLoc;
        *mDraggedGradientStop = newLoc;
    }
}


void LinearGradientApp::keyDown( KeyEvent event )
{
//    if( event.getCode() == app::KeyEvent::KEY_f ) {
//        setFullScreen( ! isFullScreen() );
//    }
}

CINDER_APP_BASIC( LinearGradientApp, RendererGl )