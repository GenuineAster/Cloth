#include "config.hpp"
#include <vector>
#include <SFML/System/Sleep.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
const sf::Vector2i win_size=
#ifdef USE_IMGUI
	{1200, 600};
	#include "imgui/imgui_SFML.h"
#else
	{800, 600};
#endif

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
	bool grabbed;
	Vector pin;
	Vector v;
	Vector p;
	Vector g;
	PrecisionType mass;
	std::vector<Constraint*> constraints;
};

void generate_cloth(Node *&grid, std::vector<Constraint*> &constraints);

int main() {
	Node *grid = nullptr;
	std::vector<Constraint*> constraints;
	generate_cloth(grid, constraints);
	sf::RenderWindow window{{1200,600}, "Cloth."};
#ifdef USE_IMGUI
	InitImGui();
#endif
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
#ifdef USE_IMGUI
		mousePressed[0] = mousePressed[1] = false;
#endif
		PrecisionType dt=clck.restart().asMicroseconds()/1e6f;
		sf::Event event;
		while(window.pollEvent(event)) {
#ifdef USE_IMGUI
			imgui_process_event(event);
#endif
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
							if((dx*dx+dy*dy)<=(ms_grb_rad*ms_grb_rad)) {
								mouse_grab.push_back(&grid[i]);
								grid[i].grabbed=true;
							}
						}
					}
					break;
				}
				case sf::Event::MouseButtonReleased: {
					if(event.mouseButton.button==sf::Mouse::Button::Left) {
						lms=false;
						for(auto &n : mouse_grab)
							n->grabbed=false;
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
		PrecisionType phys_dt=(dt/stps_pr_itr)*sim_spd;
		window.clear(sf::Color::White);
		for(int s{0};s<stps_pr_itr;++s) {
			for(int i{0};i<grd_sz;++i) {
				if(grid[i].fixed) grid[i].p=grid[i].pin;
				if(!grid[i].grabbed || !grid[i].fixed) {
					grid[i].g.x+=grvt_x*phys_dt;
					grid[i].g.y+=grvt_y*phys_dt;
					grid[i].p.x+=grid[i].v.x*phys_dt;
					grid[i].p.y+=grid[i].v.y*phys_dt;
					grid[i].p.x+=grid[i].g.x*phys_dt;
					grid[i].p.y+=grid[i].g.y*phys_dt;
					grid[i].g.x-=grid[i].g.x*phys_dt*grvt_dmp;
					grid[i].g.y-=grid[i].g.y*phys_dt*grvt_dmp;
					if(grid[i].p.x<=0.f || grid[i].p.x>=window.getSize().x) {
						grid[i].v.x=-grid[i].v.x/2.f;
						grid[i].g.x=-grid[i].g.x/2.f;
						grid[i].p.x=(grid[i].p.x<=0.f)?0.f:window.getSize().x;
					}
					if(grid[i].p.y<=0.f || grid[i].p.y>=window.getSize().y){
						grid[i].v.y=-grid[i].v.y/2.f;
						grid[i].g.y=-grid[i].g.y/2.f;
						grid[i].p.y=(grid[i].p.y<=0.f)?0.f:window.getSize().y;
					}
				}
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
						diff = ((constraints[i]->length-dist)/dist);
					else diff=dist;
					PrecisionType px = dx * diff * 0.5;
					PrecisionType py = dy * diff * 0.5;
					constraints[i]->nodes[0]->p.x+=px*phys_dt*constraint_resistance;
					constraints[i]->nodes[0]->p.y+=py*phys_dt*constraint_resistance;
					constraints[i]->nodes[1]->p.x-=px*phys_dt*constraint_resistance;
					constraints[i]->nodes[1]->p.y-=py*phys_dt*constraint_resistance;
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
			if(ms_grb_whn_stpd && lms) {
				sf::Vector2i mouse = sf::Mouse::getPosition(window);
				for(auto &n : mouse_grab) {
					n->p.x = mouse.x;
					n->p.y = mouse.y;
					n->v.x = 0.f;
					n->v.y = 0.f;
				}
			}
		}
		window.draw(lines);
		window.draw(points);
		window.setTitle("Cloth. :: " + std::to_string(1.f/dt) + "FPS");
#ifdef USE_IMGUI
		static bool show_ui = true;
		static float im_grvt[2] = {grvt_x,grvt_y};
		UpdateImGui(window);
		ImGui::Begin("Cloth Simulation Configuration", &show_ui, {500,100});
		ImGui::PushItemWidth(200.f);
		ImGui::InputInt("Steps per Itr", &stps_pr_itr);
		ImGui::SliderFloat("Simulation Speed", &sim_spd, 1.f, 100.f);
		ImGui::SliderFloat("Gravity Damp", &grvt_dmp, 0.f, 1.f);
		ImGui::InputFloat("Node Mass", &nd_mss);
		ImGui::SliderFloat2("Gravity", im_grvt, 0.f, 10.f);
		ImGui::PushItemWidth(ImGui::GetItemWidth()/2.2f);
		ImGui::InputInt("##Nodes X", &grd_sz_x);
		ImGui::SameLine();
		ImGui::InputInt("Nodes X/Y", &grd_sz_y);
		ImGui::PopItemWidth();
		ImGui::SliderFloat("Mouse Grab Radius", &ms_grb_rad, 0.f, 500.f);
		ImGui::InputFloat("Constraint Length", &constraint_length);
		ImGui::InputFloat("Constraint Resistance", &constraint_resistance);
		ImGui::InputFloat("Max Constraint Length", &max_constraint_length);
		ImGui::PushItemWidth(ImGui::GetItemWidth()/2.3f);
		ImGui::InputFloat("##Start X", &strt_x);
		ImGui::SameLine();
		ImGui::InputFloat("Start X/Y", &strt_y);
		ImGui::InputFloat("##Size X", &sz_x);
		ImGui::SameLine();
		ImGui::InputFloat("Size X/Y", &sz_y);
		ImGui::PopItemWidth();
		if(ImGui::Button("Rebuild Cloth")) {
			generate_cloth(grid, constraints);
			lines.resize(constraints.size()*2);
			points.resize(grd_sz);
			for(int i{0};i<points.getVertexCount();++i)
				points[i].color={200,200,200};
			mouse_grab.clear();
		}
		ImGui::PopItemWidth();
		ImGui::End();
		window.pushGLStates();
		ImGui::Render();
		window.popGLStates();
		grvt_x=im_grvt[0];
		grvt_y=im_grvt[1];
#endif
		window.display();
		sf::sleep(sf::milliseconds(1));
	}
	delete[] grid;
	for(auto &c : constraints)
		delete c;
	return 0;
}

void generate_cloth(Node *&grid, std::vector<Constraint*> &constraints) {
	grd_sz=grd_sz_x*grd_sz_y;
	cnstrnts_sz=grd_sz-grd_sz_x-grd_sz_y;
	if(grid != nullptr)
		delete[] grid;
	grid = new Node[grd_sz];
	for(auto &c : constraints)
		delete c;
	constraints.clear();
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
				constraints.back()->resistance=constraint_resistance;
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
				constraints.back()->resistance=constraint_resistance;
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
			grid[(y*grd_sz_x)+x].grabbed=false;
		}
	}
}
