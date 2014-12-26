#include "config.hpp"
#include <vector>
#include <SFML/System/Sleep.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

struct Vector {
	PrecisionType x,y;
	Vector(PrecisionType x=0.f, PrecisionType y=0.f):x{x},y{y}{}
};

struct Node;
struct Constraint {
	bool active;
	Node *nodes[2];
	PrecisionType length, maxlength;
	PrecisionType resistance;
};

struct Node {
	bool fixed;
	Vector pin;
	Vector v;
	Vector p;
	PrecisionType mass;
	std::vector<Constraint*> constraints;
};

int main() {
	Node *grid = new Node[grd_sz];
	std::vector<Constraint*> constraints;
	constraints.reserve(cnstrnts_sz);
	for(int x{0};x<grd_sz_x;++x) {
		for(int y{0};y<grd_sz_y;++y) {
			if(x<(grd_sz_x-1)){
				constraints.push_back(new Constraint);
				constraints.back()->nodes[0]=&grid[(y*grd_sz_x)+x];
				constraints.back()->nodes[1]=&grid[(y*grd_sz_x)+x+1];
				constraints.back()->length=constraint_length;
				constraints.back()->maxlength=max_constraint_length;
				constraints.back()->active=true;
				grid[(y*grd_sz_x)+x].constraints.push_back(constraints.back());
				grid[(y*grd_sz_x)+x+1].constraints.push_back(constraints.back());
			}
			if(y<(grd_sz_y-1)){
				constraints.push_back(new Constraint);
				constraints.back()->nodes[0]=&grid[(y*grd_sz_x)+x];
				constraints.back()->nodes[1]=&grid[((y+1)*grd_sz_x)+x];
				constraints.back()->length=constraint_length;
				constraints.back()->maxlength=max_constraint_length;
				constraints.back()->active=true;
				grid[(y*grd_sz_x)+x].constraints.push_back(constraints.back());
				grid[((y+1)*grd_sz_x)+x].constraints.push_back(constraints.back());
			}
			grid[(y*grd_sz_x)+x].mass=nd_mss;
			grid[(y*grd_sz_x)+x].p.x=strt_x+(sz_x/grd_sz_x)*x;
			grid[(y*grd_sz_x)+x].p.y=strt_y+(sz_y/grd_sz_y)*y;
			grid[(y*grd_sz_x)+x].v={0.f,0.f};
			if(!y)grid[(y*grd_sz_x)+x].pin=grid[(y*grd_sz_x)+x].p;
			else grid[(y*grd_sz_x)+x].pin={0.f,0.f};
			grid[(y*grd_sz_x)+x].fixed=!y;
		}
	}
	sf::RenderWindow window{{800,600}, "Cloth."};
	sf::VertexArray lines;
	lines.setPrimitiveType(sf::Lines);
	lines.resize(constraints.size()*2);
	sf::VertexArray points;
	points.setPrimitiveType(sf::Points);
	points.resize(grd_sz);
	for(int i{0};i<points.getVertexCount();++i)
		points[i].color={200,200,200};
	std::vector<Node*> mouse_grab;
	bool lms=false;
	sf::Clock clck;
	while(window.isOpen()) {
		PrecisionType dt=clck.restart().asMicroseconds()/1e6f;
		sf::Event event;
		while(window.pollEvent(event)) {
			switch(event.type) {
				case sf::Event::Closed: {
					window.close();
					break;
				}
				case sf::Event::Resized: {
					window.setSize(window.getSize());
					auto view = window.getView();
					view.setSize(static_cast<sf::Vector2f>(window.getSize()));
					window.setView(view);
					break;
				}
				case sf::Event::MouseButtonPressed: {
					if(event.mouseButton.button==sf::Mouse::Button::Left) {
						lms=true;
						for(int i{0};i<grd_sz;++i){
							float dx=event.mouseButton.x-grid[i].p.x;
							float dy=event.mouseButton.y-grid[i].p.y;
							if((dx*dx+dy*dy)<=ms_grb_rad)
								mouse_grab.push_back(&grid[i]);
						}
					}
					break;
				}
				case sf::Event::MouseButtonReleased: {
					if(event.mouseButton.button==sf::Mouse::Button::Left) {
						lms=false;
						mouse_grab.clear();
					}
					break;
				}
				case sf::Event::MouseMoved: {
					if(lms){
						for(auto &n : mouse_grab) {
							n->p.x = event.mouseMove.x;
							n->p.y = event.mouseMove.y;
						}
					}
					break;
				}
				default: break;
			}
		}
		PrecisionType phys_dt=dt/stps_pr_itr;
		window.clear(sf::Color::White);
		for(int s{0};s<stps_pr_itr;++s) {
			for(int i{0};i<grd_sz;++i) {
				grid[i].v.x+=(grvt_x*grid[i].mass)*phys_dt;
				grid[i].v.y+=(grvt_y*grid[i].mass)*phys_dt;
				grid[i].p.x+=grid[i].v.x*phys_dt;
				grid[i].p.y+=grid[i].v.y*phys_dt;
				if(grid[i].p.x<=0.f || grid[i].p.x>=window.getSize().x) {
					grid[i].v.x=-grid[i].v.x/2.f;
					grid[i].p.x=(grid[i].p.x<=0.f)?0.f:window.getSize().x;
				}
				if(grid[i].p.y<=0.f || grid[i].p.y>=window.getSize().y){
					grid[i].v.y=-grid[i].v.y/2.f;
					grid[i].p.y=(grid[i].p.y<=0.f)?0.f:window.getSize().y;
				}
				if(grid[i].fixed) grid[i].p=grid[i].pin;
				if(s==stps_pr_itr-1) {
					points[i].position.x = grid[i].p.x;
					points[i].position.y = grid[i].p.y;
				}
			}
			for(int i{0};i<constraints.size();++i) {
				if(constraints[i]->active) {
					PrecisionType dx = constraints[i]->nodes[0]->p.x - constraints[i]->nodes[1]->p.x;
					PrecisionType dy = constraints[i]->nodes[0]->p.y - constraints[i]->nodes[1]->p.y;
					PrecisionType dist = std::sqrt(dx*dx+dy*dy);
					if(dist>constraints[i]->maxlength) {
						constraints[i]->active=false;
						continue;
					}
					PrecisionType diff;
					if(dist>0.f)
						diff = (constraints[i]->length-dist)/dist;
					else diff=dist;
					PrecisionType px = dx * diff * 0.5;
					PrecisionType py = dy * diff * 0.5;
					constraints[i]->nodes[0]->p.x+=px;
					constraints[i]->nodes[0]->p.y+=py;
					constraints[i]->nodes[1]->p.x-=px;
					constraints[i]->nodes[1]->p.y-=py;
				}
				if(s==stps_pr_itr-1) {
					sf::Color line_color = ((constraints[i]->active)?sf::Color{150,150,150}:sf::Color{0,0,0,0});
					lines[i*2].position.x=constraints[i]->nodes[0]->p.x;
					lines[i*2].position.y=constraints[i]->nodes[0]->p.y;
					lines[i*2+1].position.x=constraints[i]->nodes[1]->p.x;
					lines[i*2+1].position.y=constraints[i]->nodes[1]->p.y;
					lines[i*2].color=lines[i*2+1].color=line_color;
				}
			}
		}
		window.draw(lines);
		window.draw(points);
		window.setTitle("Cloth. :: " + std::to_string(1.f/dt) + "FPS");
		window.display();
		sf::sleep(sf::milliseconds(10));
	}
	delete[] grid;
	return 0;
}
