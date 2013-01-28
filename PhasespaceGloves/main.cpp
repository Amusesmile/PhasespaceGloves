/*
 Joshua Dickinson - 1/18/2013
 This demonstrates how to set up an allocore app with a phasespace client running in a separate thread. We set up a number of gloves which are initialized automatically based on incoming tracker ID numbers.
 */

#include <stdio.h>
#include <cmath>
#include <vector>

//allocore includes
#include "allocore/io/al_App.hpp"
#include "allocore/al_Allocore.hpp"
#include "alloutil/al_OmniApp.hpp"

//gamma includes
#include "Gamma/Oscillator.h"
#include "Gamma/Filter.h"
#include "Gamma/Gamma.h"

//phasespace includes
#include "./alloOwl/alloOwl.h"

#include "./josh/CohenColor.h"
#include "./CAL/CAL.h"

using std::cout;
using std::endl;
using namespace al;
//using namespace gam;
using namespace cal;

struct MyApp : OmniApp {
//    Mesh mesh;
//    Light light;
    
    MyApp() {
        mesh.primitive(Graphics::TRIANGLES);
        addSphere(mesh, 1.0, 32, 32);
        for (int i = 0; i < mesh.vertices().size(); ++i) {
            float f = (float)i / mesh.vertices().size();
            mesh.color(Color(HSV(f, 1 - f, 1), 1));
        }
        mesh.generateNormals();
        light.ambient(Color(0.4, 0.9, 0.4, 1.0));
        light.pos(5, 5, 5);
        
        Image img("/Users/joshuadickinson/Documents/allo/AlloSystem/examples/joshua.dickinson/PhasespaceGloves/PhasespaceGloves/textures/grunge1.jpg");
        tex.allocate(img.array());
    }
    
    virtual ~MyApp() {}
    
    virtual void onDraw(Graphics& g) {
        light();
        // say how much lighting you want
        shader().uniform("lighting", 1.0);
        g.draw(mesh);
        
        g.antialiasing(Graphics::NICEST);
        g.depthTesting(true);
        g.blending(true);
        g.shadeModel(Graphics::SMOOTH);
        KinematicState *markerPositions = phasespaceManager.getStates();
        for (int i = 0; i < 20; ++i)
        {
            tex.bind();
            g.pushMatrix();
            g.color(cohenColor(i));
            g.translate(Vec3f(1.0,1.0,1.0*i));
            //g.translate(markerPositions[i].acceleration);
            g.rotate((360.0/20.0)*i, 0.0, 0.0, 1.0);
            
            g.begin(g.TRIANGLES);
			g.vertex(-1, -1);
			//g.vertex( 1, -1);
			g.vertex( 1,  1);
			g.vertex(-1,  1);
            g.texCoord(0,0);
			g.texCoord(.1,0);
			g.texCoord(.1,.1);
			g.texCoord(0,.1);
            g.end();
            
            //g.draw(mesh);
            g.popMatrix();
            tex.unbind();
        }

    }
    
    virtual void onAnimate(al_sec dt) {
        //light.pos(nav().pos());
        //std::cout << dt << std::endl;
    }
    
    virtual void onSound(AudioIOData& io)
    {
        if(rnd::uniform(0.0, 10.0) < 0.1)
        {
            pulsaret.trigger();
        }
        while (io())
        {
            float outputSample = rand()/(float)RAND_MAX;
            doubleLR tempVal = pulsaret.stereo(doubleLR(outputSample));
            io.out(0) = tempVal.L;
            io.out(1) = tempVal.R;
        }
    }

    
    virtual void onMessage(osc::Message& m) {
        OmniApp::onMessage(m);
    }
    
    virtual bool onKeyDown(const Keyboard& k)
    {
        if (k.key() == ' ')
        {
            printf("another color\n");
            float temp = rnd::uniform(0.0, 0.7);
            cohenColor.set(rand()%2, rand()%3, 10, temp, temp+.3);
        }
        return true;
    }

    PhasespaceManager phasespaceManager;
    SearchPaths searchpaths;
    Material material;
    Light light;
    Mesh mesh;
    Color color;
    CohenColor cohenColor;
    std::vector<Pose> poses;
    Texture tex;
    Image image;
    
    // Vec3f contains 3 floating point numbers that may be accessed in many ways
    // that are convienient for simulations and graphics. many operators and
    // methods are defined for Vec: +, -, +=, -=, ==, !=, [], elems(), mag(),
    // normalize(), norm(), clip(), wrap(), lerp(). for this code, position[i] is
    // a Vec3f that represents the 3d cartsian coordinate of the i-th sphere. for
    // example, the y coordinate of the 7th sphere is spelled "position[7].y".
    Vec3f position[20];
    Pulsaret pulsaret;

};

//struct MyApp : OmniApp
//{
//    MyApp()
//    {
//        //phasespaceManager = PhasespaceManager::master();
//        //phasespaceManager.startThread("192.168.0.127");
//        
//        Image img("/Users/joshuadickinson/Documents/allo/AlloSystem/examples/joshua.dickinson/PhasespaceGloves/PhasespaceGloves/textures/grunge1.jpg");
//        tex.allocate(img.array());
//
//        color.set(HSV(0.5,0.3,0.6), 1.0);
//        
//        //Image img(searchpaths.find("grunge1.jpg").filepath());
//        //Image img("./textures/grunge1.jpg");
//        //tex.allocate(img.array());
//        
//        // this can be read "move the viewer position back 10 units."
//        //nav().pos(0, 0, 10);
//        
//        // set our mesh to render using the OpenGL primitive called "triangles".
//        // this tells the GPU to fill in triangles with color or shading or both.
//        //
//        m.primitive(Graphics::TRIANGLES);
//        
////        m.vertex(0.0, 0.0, 0.0);
////        m.vertex(1.0, 0.0, 0.0);
////        m.vertex(0.0, 1.0, 0.0);
////        //m.color(cohenColor(9));
//        addSphere(m, 1.0, 32, 32);
//        for (int i = 0; i < m.vertices().size(); ++i) {
//            float f = (float)i / m.vertices().size();
//            m.color(Color(HSV(f, 1 - f, 1), 1));
//        }
//        
//        
//        for(int i = 0;i<20;i++)
//        {
//            poses.push_back(Pose());
//        }
//        
//        // this scales (multiplies) each vertex by the value. the CPU executes
//        // these multiplications, not the GPU.
//        //
//        m.scale(0.8);
//        
//        // this uses math to figure out the normals for our mesh. this is critical
//        // to lighting/shading. also, we must do this *after* all the verticies are
//        // added and scaled.
//        //
//        m.generateNormals();
//        
//        light.ambient(Color(0.4, 0.9, 0.4, 1.0));
//        //light.pos(5, 5, 5);
//        
//        for (int i = 0; i < 20; ++i)
//        {
//            position[i] = Vec3f(0.0, i/6.0, i*0.1);
//        }
//    }
//    
//    virtual void onAnimate(double dt)
//    {
//        for(int i = 0;i<poses.size();i++)
//        {
//            Quat<float> tempQuat = Quat<float>(rnd::uniform(0.0,1.0), rnd::uniform(0.0,1.0), rnd::uniform(0.0,1.0), rnd::uniform(0.0,1.0));
//            //poses.at(i).lerp(tempQuat, 0.5);
//        }
//    }
//    
//    virtual void onDraw(Graphics& g, const Viewpoint& v)
//    {
//        material();
//        light();
//        // say how much lighting you want
//        shader().uniform("lighting", 1.0);
//        
//        g.antialiasing(Graphics::NICEST);
//        g.depthTesting(true);
//        g.blending(true);
//
////        //Graham combo
////        g.depthTesting(1);
////        g.depthMask(0);
////        g.blending(1);
////        g.blendModeAdd();
//        
//        //g.enable(Graphics::COLOR_MATERIAL);
//        g.shadeModel(Graphics::SMOOTH);
//        g.blendModeTrans();
//        
//        KinematicState *markerPositions = phasespaceManager.getStates();
//        for (int i = 0; i < 20; ++i)
//        {
//            tex.bind();
//            g.pushMatrix();
//            g.color(cohenColor(i));
//            //g.translate(position[i]);
//            g.translate(markerPositions[i].acceleration);
//            //g.rotate((360.0/20.0)*i, 0.0, 0.0, 1.0);
//            
//            g.begin(g.TRIANGLES);
//			g.vertex(-1, -1);
//			//g.vertex( 1, -1);
//			g.vertex( 1,  1);
//			g.vertex(-1,  1);
//            g.texCoord(0,0);
//			g.texCoord(.1,0);
//			g.texCoord(.1,.1);
//			g.texCoord(0,.1);
//            g.end();
//            
//            g.draw(m);
//            g.popMatrix();
//            tex.unbind();
//        }
//
//    }
//    virtual void onSound(AudioIOData& io)
//    {
//        while (io())
//        {
//            float outputSample = rand()/(float)RAND_MAX;
//            doubleLR tempVal = pulsaret.stereo(doubleLR(outputSample));
//            if(rnd::uniform(0.0, 10.0) < 0.1)
//            {
//                
//            }
//            io.out(0) = tempVal.L;
//            io.out(1) = tempVal.R;
//        }
//    }
//
//    virtual void onMessage(osc::Message& m) {
//        OmniApp::onMessage(m);
//    }
//    
//    virtual void onKeyDown(const ViewpointWindow&, const Keyboard& k)
//    {
//        if (k.key() == ' ')
//        {
//            float temp = rnd::uniform(0.0, 0.7);
//            cohenColor.set(rand()%2, rand()%3, 10, temp, temp+.3);
//        }
//        
//    }
//
//    PhasespaceManager phasespaceManager;
//    SearchPaths searchpaths;
//    Material material;
//    Light light;
//    Mesh m;
//    Color color;
//    CohenColor cohenColor;
//    std::vector<Pose> poses;
//    Texture tex;
//    Image image;
//    
//    // Vec3f contains 3 floating point numbers that may be accessed in many ways
//    // that are convienient for simulations and graphics. many operators and
//    // methods are defined for Vec: +, -, +=, -=, ==, !=, [], elems(), mag(),
//    // normalize(), norm(), clip(), wrap(), lerp(). for this code, position[i] is
//    // a Vec3f that represents the 3d cartsian coordinate of the i-th sphere. for
//    // example, the y coordinate of the 7th sphere is spelled "position[7].y".
//    Vec3f position[20];
//    Pulsaret pulsaret;
//};


//MAIN
int main()
{
    MyApp myApp;
    
    myApp.start();
    return 0;
}