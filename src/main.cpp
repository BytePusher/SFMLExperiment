#include <complex>
#include <SFML/Graphics.hpp>

#include "Utility/Utility.h"

#define WIDTH 800
#define HEIGHT 800

void UpdateSprite(sf::Sprite& sprite, sf::Texture& texture, uint8_t * buffer);

void update_pixmap(double x_start, double x_stop, double y_start, double y_stop,
                   uint8_t* buffer, uint32_t bytes_per_row, double* x_l,
                   double* y_l);


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
				    x_start = -2;
					x_stop = 2;
					y_start = -2;
					y_stop = 2;
				}
 				redraw = 1;
                break;
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

void update_pixmap(double x_start, double x_stop, double y_start, double y_stop,
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
			std::complex<double> c(x_l[x], y_l[y]);
			std::complex<double>  z = 0;
 
			uint32_t offset = x * 4 + y * bytes_per_row;
 
 			uint32_t i = 0;
            double mg = 0.0;
			buffer[offset + 0] = buffer[offset + 1] = buffer[offset + 2] = 0;
			buffer[offset + 2] = 128;
			while (i < 25/*50*/)
            {
				z = z * z + c;
				double img = z.imag();
				double real = z.real();                
				/*double*/mg = real * real + img * img;
				if (mg > 4.0)
				{
					int k = (int)(mg * 100);
					buffer[offset + 0] = (i * 9 * 2) % 255;
					buffer[offset + 1] = (i * 5 * 2) % 255;
					buffer[offset + 2] = (i * 3 * 2) % 255;
					break;
				}
/*				
                else if (mg < 0.5) 
                {
                    int k = (int)(mg * 100);
                    buffer[offset + 0] = (k * i * 2) % 255;
                    buffer[offset + 1] = (k * i * 2) % 255;
                    buffer[offset + 2] = (k * i * 9) % 255;
                }                
                else if (mg < 1) 
                {
                    int k = (int)(mg * 100);
                    buffer[offset + 0] = (k * i * 8) % 255;
                    buffer[offset + 1] = (k * i * 2) % 255;
                    buffer[offset + 2] = (k * i * 8) % 255;
                }
  */              
    			i++;                
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