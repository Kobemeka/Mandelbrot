/*
    You can zoom in by pressing *
    You can zoom out by pressing /
    You can increase maximum iteration value by pressing +
    You can decrease maximum iteration value by pressing +-
    You can pan up by pressing W or up arrow key
    You can pan down by pressing S or down arrow key
    You can pan right by pressing D or right arrow key
    You can pan left by pressing A or left arrow key

*/

// including SFML library and iostream for debugging
#include <SFML/Graphics.hpp>
#include <iostream>

// to make the window full screen Width and height are defined by aspect ratio 16-9 
double aspect_ratio = 16/9;
int WIDTH = 1920;
int HEIGHT = (int) WIDTH/aspect_ratio;

// initial center x and y
double centerx = -0.0;
double centery = -0.0;
// the distance from center to edges for x and y
// i.e the window shows -m_x to m_x as the real axis and -m_y to m_y as the imaginary axis
double m_x = 0.2; 
double m_y = 0.2;

// initial max itearation
double MAX_ITER = 50.;

// when a user zooms, the m_x and m_y will be multiplied or divied by zoom coefficient (zoom in / zoom out)
double zoom_coef = 0.915;


// to make it look better i want to use hsv color space but the sfml library only accepts the rgb/a color space
// so that i need a hsv to rgb converter

// taken from https://en.sfml-dev.org/forums/index.php?topic=7313.msg48304#msg48304
// hue: 0-360°; sat: 0.f-1.f; val: 0.f-1.f

sf::Color hsv(int hue, float sat, float val)
{
  hue %= 360;
  while(hue<0) hue += 360;

  if(sat<0.f) sat = 0.f;
  if(sat>1.f) sat = 1.f;

  if(val<0.f) val = 0.f;
  if(val>1.f) val = 1.f;

  int h = hue/60;
  float f = float(hue)/60-h;
  float p = val*(1.f-sat);
  float q = val*(1.f-sat*f);
  float t = val*(1.f-sat*(1-f));

  switch(h)
  {
    default:
    case 0:
    case 6: return sf::Color(val*255, t*255, p*255);
    case 1: return sf::Color(q*255, val*255, p*255);
    case 2: return sf::Color(p*255, val*255, t*255);
    case 3: return sf::Color(p*255, q*255, val*255);
    case 4: return sf::Color(t*255, p*255, val*255);
    case 5: return sf::Color(val*255, p*255, q*255);
  }
}

// this function map the mouse position to the drawing coordinates

double w2s(double pos, double center,double margin,int real_length){
    // real length can be WIDTH or HIGHT
    // the returning value must be a double so convert int real_length to double
    double rl = (double) real_length;
    // return a double type value
    // the width or the height of the drawing is 2*m_x or 2*m_y
    // divide it by window length (HEIGHT or WIDTH)
    // then multiply position by itself
    // it gives a value between 0 to 2*m_x or 2*m_y
    // but the result must be between -m, m (x or y) relative to the center
    // so substract margin then add center to the resulting value
    return (double) pos*2.*margin/rl - margin + center;
}
// this function changes the maximum iteration (MAX_ITER)
void change_max_iter(double *mi, double amount){
    // if max iter is between 10 or 100
    // this function increases or decreases the value
    if (*mi + amount >= 10. && *mi + amount <= 100.)
    {
        *mi += amount;
    }
    
}
// this function changes the center of the drawing by tenth of the margin value in the given direction
void pan(double *cx, double *cy, double *mx, double *my, int dirx,int diry){

    *cx += dirx*(*mx)/10;
    *cy += diry*(*my)/10;
}
// these two function (zoom in and zoom out) changes the margin values by zoom coefficient
// TODO: zoom in or zoom out by keeping the center values
void zoom_in(double *cnx, double *cny, double *omx, double *omy,double mpx, double mpy){

    *omx *= zoom_coef;
    *omy *= zoom_coef; 
    
    // *cnx = w2s(mpx,*cnx,m_x,WIDTH) ;
    // *cny = w2s(mpy,*cny,m_y,HEIGHT);


}
void zoom_out(double *cnx, double *cny, double *omx, double *omy,double mpx, double mpy){
    *omx /= zoom_coef;
    *omy /= zoom_coef; 

    // *cnx = w2s(mpx,*cnx,m_x,WIDTH);

    // *cny = w2s(mpy,*cny,m_y,HEIGHT);

}
// draws mandelbrot
// it is not optimized
// TODO: Optimize the code
void draw_mandelbrot(double cx, double cy, double mx, double my,sf::Image &image, sf::Texture &texture, sf::Sprite &sprite){
    

    double re,im,initial_re,initial_im,real_term,imaginary_term;
    int iter;
    int h;
    float s,v;
    double XS = cx - mx;
    double YS = cy - my;

    for (int x = 0; x < WIDTH; x++)
    {

        for (int y = 0; y < HEIGHT; y++)
        {
            iter = 0;

            initial_re = XS + x*mx*2.f/WIDTH;
            initial_im = YS + y*my*2.f/HEIGHT;

            re = XS + x*mx*2.f/WIDTH;
            im = YS + y*my*2.f/HEIGHT;

            while(iter < MAX_ITER){
                
                real_term = re * re - im * im;
                imaginary_term = 2 * re * im;

                re = real_term + initial_re;
                im = imaginary_term + initial_im;

                if(std::abs(re + im) > 2) break;

                iter++;
            }
            if(iter >= MAX_ITER){
                h = 0;
                s = 0;
                v = 0;
            }else{
                float ratio = iter/MAX_ITER;
                int intratio = (int) ratio;

                h = 360*ratio;
                s = 0.8f;
                v = 0.8f;
            }
            sf::Color pixel_color;
            pixel_color = hsv(h,s,v);

            image.setPixel(x,y,pixel_color);
        }
        
    }
    texture.loadFromImage(image);
    sprite.setTexture(texture);

}

int main(){

    sf::RenderWindow window;
    window.create(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot Set");
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;
    
    image.create(WIDTH,HEIGHT,sf::Color::Black);

    
    draw_mandelbrot(centerx,centery,m_x,m_y,image,texture,sprite);
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            
            // if a key pressed
            else if(event.type == sf::Event::KeyPressed){
                // if the pressed key is + (add)
                // increaes the maximum iteration value
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Add)){
                    change_max_iter(&MAX_ITER,5.);
                    draw_mandelbrot(centerx,centery,m_x,m_y,image,texture,sprite);
                }
                // if the pressed key is - (substract)
                // decreases the maximum iteration value
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract)){
                    change_max_iter(&MAX_ITER,-5.);
                    draw_mandelbrot(centerx,centery,m_x,m_y,image,texture,sprite);
                }
                // if the pressed key is * (mulitply)
                // zooms in
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Multiply)){
                    zoom_in(&centerx,&centery,&m_x,&m_y,centerx,centery);
                    draw_mandelbrot(centerx,centery,m_x,m_y,image,texture,sprite);

                }
                // if the pressed key is / (divide)
                // zooms out
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Divide)){
                    zoom_out(&centerx,&centery,&m_x,&m_y,centerx,centery);
                    draw_mandelbrot(centerx,centery,m_x,m_y,image,texture,sprite);
                }
                // if the pressed key is up arrow key or W
                // panning up
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
                    pan(&centerx,&centery,&m_x,&m_y,0,-1);
                    draw_mandelbrot(centerx,centery,m_x,m_y,image,texture,sprite);
                }
                // if the pressed key is down arrow key or S
                // panning down
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
                    pan(&centerx,&centery,&m_x,&m_y,0,1);
                    draw_mandelbrot(centerx,centery,m_x,m_y,image,texture,sprite);
                }
                // if the pressed key is right arrow key or D
                // panning right
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
                    pan(&centerx,&centery,&m_x,&m_y,1,0);
                    draw_mandelbrot(centerx,centery,m_x,m_y,image,texture,sprite);
                }
                // if the pressed key is left arrow key or A
                // panning left
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
                    pan(&centerx,&centery,&m_x,&m_y,-1,0);
                    draw_mandelbrot(centerx,centery,m_x,m_y,image,texture,sprite);
                }
            }
        }
        window.clear();
        
        window.draw(sprite);
        window.display();
    }
    return 0;
}