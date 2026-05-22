#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "Utility/Utility.h"

#define WIDTH 800
#define HEIGHT 800


void complex_squared( double r_in, double i_in, double * r_out, double * i_out );
void complex_cubed( double r_in, double i_in, double * r_out, double * i_out );
void complex_divide( double r_num, double i_num, double r_den, double i_den, double * r_out, double * i_out );
double complex_diff_squared(double r1, double i1, double r2, double i2);

void set_initial_values(double * px_start, double * px_stop, double * py_start, double * py_stop);

void UpdateSprite(sf::Sprite& sprite, sf::Texture& texture, uint8_t * buffer);

void update_pixmap(double x_start, double x_stop, double y_start, double y_stop,
                   uint8_t* buffer, uint32_t bytes_per_row, double* x_l,
                   double* y_l);

void update_pixmap_mandelbrot(double x_start, double x_stop, double y_start, double y_stop,
                   uint8_t* buffer, uint32_t bytes_per_row, double* x_l,
                   double* y_l);

void update_pixmap_julia(double x_start, double x_stop, double y_start, double y_stop,
                   uint8_t* buffer, uint32_t bytes_per_row, double* x_l,
                   double* y_l);
				   
void update_pixmap_newton(double x_start, double x_stop, double y_start, double y_stop,
                   uint8_t* buffer, uint32_t bytes_per_row, double* x_l,
                   double* y_l);				   
				   
bool fConvergeColorBlack = false;

enum class fractal_type {fractal_mandelbrot, fractal_julia, fractal_newton};

fractal_type  cur_fractal_type = fractal_type::fractal_mandelbrot;

// Initial Values for c
// Index 0: c = -0.70176 - 0.3842i\): Produces a detailed, classic, spiral-like fractal.
// Index 1: c = -0.8 + 0.156i\): Generates a "sea-horse" style fractal.
// Index 2: c = 0.285 + 0.01i\): Produces a "douady's rabbit" style fractal.
// Index 3: c = -0.12256 + 0.74486i\): Produces a "douady's rabbit" style fractal.
// Index 4: c = -0.4 + 0.6i: Another popular, intricate shape.
// Index 5: c = -1.476 Produces a classic dendrite fractal.

double julia_cr_init[] = {-0.70176, -0.8,   0.285, -0.12256, -0.4, -1.476, -0.03051, -0.2667, -0.40193, -0.57976, -0.38506};
double julia_ci_init[] = {-0.3842,   0.156, 0.01,   0.74486,  0.6,  0.0,   -0.65586, -0.65024, 0.67769, -0.61587, -0.6385};
int  nJuliaInitIndex = 0;

bool fBurningShipMandelbrot = false;

int main()
{
    uint32_t bytes_per_row = WIDTH * 4;
	uint32_t total_bytes = HEIGHT * bytes_per_row;
 
	uint8_t* buffer = (uint8_t*)malloc(total_bytes);
	if (buffer == NULL) 
	{
		// TBD: Error
		return 0;
	}

    auto window = sf::RenderWindow(sf::VideoMode({WIDTH, HEIGHT}), "CMake SFML Project");
    window.setFramerateLimit(144);

    sf::Image image(sf::Vector2u(WIDTH, HEIGHT), sf::Color::Red);
    sf::Texture texture;
    auto textureLoaded = texture.loadFromImage(image);
    sf::Sprite sprite(texture);

 	double x_start = -2;
	double x_stop = 1;
 
	double y_start = -1.5;
	double y_stop = 2;

	set_initial_values(&x_start, &x_stop, &y_start, &y_stop);
 
	double* x_l = (double *)malloc(sizeof(double) * WIDTH);
	if (x_l == NULL) {
		free(buffer);
		return 0;
	}
	double* y_l = (double *)malloc(sizeof(double) * HEIGHT);
	if (y_l == NULL) {
		free(buffer);
		return 0;
	}
 
	int quit = 0;
	int redraw = 1;
	int zooming = 0;
 
	uint32_t x = 0;
	uint32_t y = 0;
 
	double cx = 0;
	double cy = 0;
 
	int zoom_frame_count = 5;
	double zoom = 0.0;

    update_pixmap(x_start, x_stop, y_start, y_stop, buffer, bytes_per_row, x_l, y_l);

    UpdateSprite(sprite, texture, buffer);

	
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto* mouseClick = event->getIf<sf::Event::MouseButtonPressed>())
            {

				x = mouseClick->position.x;
				y = mouseClick->position.y;

                if(mouseClick->button == sf::Mouse::Button::Left)
                {
					cx = x_start + (double)x / WIDTH * (x_stop - x_start);
					cy = y_start + (double)y / HEIGHT * (y_stop - y_start);
 
					double width = (x_stop - x_start) * 0.5;
 
					zoom =
					    (double)pow((width / ((x_stop - x_start))),
					                ((double)1 / (double)zoom_frame_count));
 
					zooming = 1;
				} 
                else 
                {
					set_initial_values(&x_start, &x_stop, &y_start, &y_stop);
				}
 				redraw = 1;
                break;
            }
			else if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>())
			{
				if(keyPress->code == sf::Keyboard::Key::Space)
				{
					fConvergeColorBlack = !fConvergeColorBlack;
 					redraw = 1;					
				}
				if(keyPress->code == sf::Keyboard::Key::J)
				{
					if(cur_fractal_type == fractal_type::fractal_julia)
					{
						nJuliaInitIndex++;
						if(nJuliaInitIndex >= sizeof julia_cr_init / sizeof julia_cr_init[0])
						{
							nJuliaInitIndex = 0;
						}
					}
					else
					{
						cur_fractal_type = fractal_type::fractal_julia;
					}
 					redraw = 1;					
				}
				else if(keyPress->code == sf::Keyboard::Key::M)
				{
					if(cur_fractal_type == fractal_type::fractal_mandelbrot)
					{
						fBurningShipMandelbrot = !fBurningShipMandelbrot;						
					}
					else
					{
						cur_fractal_type = fractal_type::fractal_mandelbrot;
					}
 					redraw = 1;					
				}
				else if(keyPress->code == sf::Keyboard::Key::N)
				{
					if(cur_fractal_type == fractal_type::fractal_newton)
					{
						// Do nothing (for now)
					}
					else
					{
						cur_fractal_type = fractal_type::fractal_newton;
					}
 					redraw = 1;					
				}
			}
        }
       
		if (redraw) 
        {
			update_pixmap(x_start, x_stop, y_start, y_stop, buffer,
			              bytes_per_row, x_l, y_l);
            UpdateSprite(sprite, texture, buffer);                          
			redraw = 0;
		}
 
		if (zooming)
        {
			double width = (x_stop - x_start) * zoom;
			double height = (y_stop - y_start) * zoom;
 
			x_start = cx - width / 2;
			x_stop = cx + width / 2;
 
			y_start = cy - height / 2;
			y_stop = cy + height / 2;
			redraw = 1;
			zoom_frame_count--;
			if (zoom_frame_count == 0) 
            {
			    redraw = 0;
				zoom_frame_count = 5;
				zooming = 0;
			}
		}
        window.clear();
        window.draw(sprite);
        window.display();
    }
	free(buffer);
	free(x_l);
	free(y_l);
}
void set_initial_values(double * px_start, double * px_stop, double * py_start, double * py_stop)
{
	if(cur_fractal_type == fractal_type::fractal_julia)
	{
		*px_start = -2;
		*px_stop = 2;
		
		*py_start = -2;
		*py_stop = 2;
	}
	else if(cur_fractal_type == fractal_type::fractal_mandelbrot)
	{
		*px_start = -2;
		*px_stop = 1;
		
		*py_start = -1.5;
		*py_stop = 2;
	}
	else if(cur_fractal_type == fractal_type::fractal_newton)
	{
		*px_start = -2;
		*px_stop = 2;
		
		*py_start = -2;
		*py_stop = 2;
	}
}

void update_pixmap(double x_start, double x_stop, double y_start, double y_stop,
                   uint8_t* buffer, uint32_t bytes_per_row, double* x_l,
                   double* y_l) 
{
	if(cur_fractal_type == fractal_type::fractal_julia)
	{
		update_pixmap_julia(x_start, x_stop, y_start, y_stop,
                            buffer, bytes_per_row, x_l, y_l); 
	}
   else if(cur_fractal_type == fractal_type::fractal_mandelbrot)
   {
		update_pixmap_mandelbrot(x_start, x_stop, y_start, y_stop,
                                 buffer, bytes_per_row, x_l, y_l); 
	}
	if(cur_fractal_type == fractal_type::fractal_newton)
	{
		update_pixmap_newton(x_start, x_stop, y_start, y_stop,
                                 buffer, bytes_per_row, x_l, y_l); 							
	}

}

void update_pixmap_mandelbrot(double x_start, double x_stop, double y_start, double y_stop,
                   uint8_t* buffer, uint32_t bytes_per_row, double* x_l,
                   double* y_l) 
{
	double x_step = (x_stop - x_start) / (double)(WIDTH - 1);
	double y_step = (y_stop - y_start) / (double)(HEIGHT - 1);
 
 	x_l[0] = x_start;
	for (uint32_t i = 1; i < WIDTH; i++) 
	{
		x_l[i] = x_l[i - 1] + x_step;
	}
 
	y_l[0] = y_start;
	for (uint32_t i = 1; i < HEIGHT; i++) 
	{
		y_l[i] = y_l[i - 1] + y_step;
	}

	for (uint32_t y = 0; y < HEIGHT; y++) 
	{
		for (uint32_t x = 0; x < WIDTH; x++) 
		{
	
			double cr = x_l[x];
			double ci = y_l[y];
			double zr = 0;
			double zi = 0;
 
			uint32_t offset = x * 4 + y * bytes_per_row;
 
 			uint32_t i = 0;
            double mg = 0.0;
			buffer[offset + 0] = buffer[offset + 1] = buffer[offset + 2] = 0;
			buffer[offset + 2] = 128;
			while (i < 50)
            {
				if(fBurningShipMandelbrot)
				{
					zr = fabs(zr);
					zi = fabs(zi);
				}

				double new_zr = ((zr * zr) - (zi*zi)) + cr;
				double new_zi = (2.0 * zr * zi) + ci;
				zr = new_zr;
				zi = new_zi;
				mg =(zr * zr) + (zi * zi);
				if (mg > 4.0)
				{
					int k = (int)(mg * 100);
					buffer[offset + 0] = (i * 9 * 2) % 255;
					buffer[offset + 1] = (i * 5 * 2) % 255;
					buffer[offset + 2] = (i * 3 * 2) % 255;
					break;
				}
				
				if(fConvergeColorBlack == false)
				{
					if (mg < 1) 
					{
						int k = (int)(mg * 100);
						buffer[offset + 0] = (k * i * 8) % 255;
						buffer[offset + 1] = (k * i * 2) % 255;
						buffer[offset + 2] = (k * i * 8) % 255;
					}
					if (mg < 0.5) 
					{
						int k = (int)(mg * 100);
						buffer[offset + 0] = (k * i * 2) % 255;
						buffer[offset + 1] = (k * i * 2) % 255;
						buffer[offset + 2] = (k * i * 9) % 255;
					}        
					if (mg < 0.25) 					
					{
						int k = (int)(mg * 100);
						buffer[offset + 0] = (k * i * 2) % 255;
						buffer[offset + 1] = (k * i * 9) % 255;
						buffer[offset + 2] = (k * i * 2) % 255;
					}       
					if (mg < 0.125) 					
					{
						int k = (int)(mg * 100);
						buffer[offset + 0] = (k * i * 9) % 255;
						buffer[offset + 1] = (k * i * 2) % 255;
						buffer[offset + 2] = (k * i * 2) % 255;
					}       


				}
    			i++;                
            }                

		}
	}
}

void update_pixmap_julia(double x_start, double x_stop, double y_start, double y_stop,
                   uint8_t* buffer, uint32_t bytes_per_row, double* x_l,
                   double* y_l) 
{
	double x_step = (x_stop - x_start) / (double)(WIDTH - 1);
	double y_step = (y_stop - y_start) / (double)(HEIGHT - 1);
 
 	x_l[0] = x_start;
	for (uint32_t i = 1; i < WIDTH; i++) 
	{
		x_l[i] = x_l[i - 1] + x_step;
	}
 
	y_l[0] = y_start;
	for (uint32_t i = 1; i < HEIGHT; i++) 
	{
		y_l[i] = y_l[i - 1] + y_step;
	}

	for (uint32_t y = 0; y < HEIGHT; y++) 
	{
		for (uint32_t x = 0; x < WIDTH; x++) 
		{
	
			double zr = x_l[x];
			double zi = y_l[y];

			double cr = julia_cr_init[nJuliaInitIndex];
			double ci = julia_ci_init[nJuliaInitIndex];

			uint32_t offset = x * 4 + y * bytes_per_row;
 
 			uint32_t i = 0;
            double mg = 0.0;
			buffer[offset + 0] = buffer[offset + 1] = buffer[offset + 2] = 0;
			buffer[offset + 2] = 128;
			while (i < 50)
            {
				double new_zr = ((zr * zr) - (zi*zi)) + cr;
				double new_zi = (2.0 * zr * zi) + ci;
				zr = new_zr;
				zi = new_zi;

				mg =(zr * zr) + (zi * zi);
				if (mg > 4.0)
				{
					int k = (int)(mg * 100);
					buffer[offset + 0] = (i * 9 * 2) % 255;
					buffer[offset + 1] = (i * 5 * 2) % 255;
					buffer[offset + 2] = (i * 3 * 2) % 255;
					break;
				}
				
				if(fConvergeColorBlack == false)
				{

					if (mg < 1) 
					{
						int k = (int)(mg * 100);
						buffer[offset + 0] = (k * i * 8) % 255;
						buffer[offset + 1] = (k * i * 2) % 255;
						buffer[offset + 2] = (k * i * 8) % 255;
					}
					if (mg < 0.5) 
					{
						int k = (int)(mg * 100);
						buffer[offset + 0] = (k * i * 2) % 255;
						buffer[offset + 1] = (k * i * 2) % 255;
						buffer[offset + 2] = (k * i * 9) % 255;
					}        
					if (mg < 0.25) 					
					{
						int k = (int)(mg * 100);
						buffer[offset + 0] = (k * i * 2) % 255;
						buffer[offset + 1] = (k * i * 9) % 255;
						buffer[offset + 2] = (k * i * 2) % 255;
					}       
					if (mg < 0.125) 					
					{
						int k = (int)(mg * 100);
						buffer[offset + 0] = (k * i * 9) % 255;
						buffer[offset + 1] = (k * i * 2) % 255;
						buffer[offset + 2] = (k * i * 2) % 255;
					}       


				}
    			i++;                
            }                

		}
	}
}

void update_pixmap_newton(double x_start, double x_stop, double y_start, double y_stop,
                   uint8_t* buffer, uint32_t bytes_per_row, double* x_l,
                   double* y_l) 
{
	double x_step = (x_stop - x_start) / (double)(WIDTH - 1);
	double y_step = (y_stop - y_start) / (double)(HEIGHT - 1);
 
	x_l[0] = x_start;
	for (uint32_t i = 1; i < WIDTH; i++) 
	{
		x_l[i] = x_l[i - 1] + x_step;
	}
 
	y_l[0] = y_start;
	for (uint32_t i = 1; i < HEIGHT; i++) 
	{
		y_l[i] = y_l[i - 1] + y_step;
	}
	double half_root_three = 0.5 * sqrt(3.0);

	//Roots of z^3 - 1
	double newton_root_r[] = {1.0, -0.5,            -0.5 };
	double newton_root_i[] = {0.0, half_root_three, -half_root_three };

	for (uint32_t y = 0; y < HEIGHT; y++) 
	{
		for (uint32_t x = 0; x < WIDTH; x++) 
		{
			double zr = x_l[x];
			double zi = y_l[y];
 
			uint32_t offset = x * 4 + y * bytes_per_row;
 
 			uint32_t i = 0;
            double mg = 0.0;
			buffer[offset + 0] = buffer[offset + 1] = buffer[offset + 2] = 0;
			buffer[offset + 2] = 128;

			bool converged = false;

			int nRoot = -1;

			while (i < 15)
            {
				double cubed_r;
				double cubed_i;
				double squared_r;
				double squared_i;
				double num_r;
				double num_i;				
				double den_r;
				double den_i;	
				double quot_r;
				double quot_i;			

				complex_cubed(zr, zi, &cubed_r, &cubed_i);
				complex_squared(zr, zi, &squared_r, &squared_i);
				complex_divide(cubed_r - 1.0, cubed_i, 3.0 * squared_r, 3.0 * squared_i, &quot_r, &quot_i);
				zr = zr - quot_r;
				zi = zi - quot_i;
		
				for(int root_ndx = 0; root_ndx < sizeof newton_root_r / sizeof newton_root_r[0]; ++root_ndx)
				{
					if(complex_diff_squared(zr, zi, newton_root_r[root_ndx], newton_root_i[root_ndx]) < 0.01)
					{
						converged = true;
						nRoot = root_ndx;						
						break;
					}
				}
				if(converged)
				{
					break;
				}
    			i++;                
            }                
			
			if(nRoot == 0)
			{
				buffer[offset + 0] = 255 - (i * 25);
				buffer[offset + 1] = 0;
				buffer[offset + 2] = 0;
			}
			else if(nRoot == 1)
			{
				buffer[offset + 0] = 0;
				buffer[offset + 1] = 255 - (i * 25);
				buffer[offset + 2] = 0;
			}
			else if(nRoot == 2)
			{
				buffer[offset + 0] = 0;
				buffer[offset + 1] = 0;
				buffer[offset + 2] = 255 - (i * 25);
			}
			else
			{
				buffer[offset + 0] = 0;
				buffer[offset + 1] = 0;
				buffer[offset + 2] = 0;
			}
		}
	}
}

void UpdateSprite(sf::Sprite& sprite, sf::Texture& texture, uint8_t * buffer)
{
    sf::Image image(sf::Vector2u(WIDTH, HEIGHT), buffer);    

    auto textureLoaded = texture.loadFromImage(image);
    sprite.setTexture(texture);                
    sprite.setPosition({0.f, 0.f});        
}


void complex_squared( double r_in, double i_in, double * r_out, double * i_out )
{
	*r_out = (r_in * r_in) - (i_in * i_in);
	*i_out = (2.0 * r_in * i_in);
}

void complex_cubed( double r_in, double i_in, double * r_out, double * i_out )
{
	*r_out = (r_in * r_in * r_in) - (3.0 * r_in * i_in * i_in);
	*i_out = (3.0 * r_in * r_in * i_in) - (i_in * i_in * i_in);
}

void complex_divide( double r_num, double i_num, double r_den, double i_den, double * r_out, double * i_out )
{
	double denom = (r_den * r_den) + (i_den * i_den);

	*r_out = ((r_num * r_den) + (i_num * i_den)) / denom;
	*i_out = ((i_num * r_den) - (r_num * i_den)) / denom;
}

double complex_diff_squared(double r1, double i1, double r2, double i2)
{
	double r_diff = r1 - r2;
	double i_diff = i1 - i2;

	return (r_diff * r_diff) + (i_diff * i_diff);
}
