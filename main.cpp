#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/undirected_dfs.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <utility>

//Vector Struct
struct v
{
    std::string binaryCode= "";
    std::string location = "";
    char north = '0';
    char east = '0';
    char south = '0';
    char west = '0';
    char up = '0';
    char down = '0';
};

//Edge Struct
struct e
{
    char direction;
};
std::vector<boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, v, e>::edge_descriptor>  path;

//getPath struct for DFS visitor
struct getPath : public boost::default_dfs_visitor
{
    template<class Edge, class Graph>
    void tree_edge(Edge E, Graph G)
    {
        path.push_back(E);
    }
};


int main()
{
    using namespace boost;

    adjacency_list<vecS, vecS, undirectedS, v, e> g;
    std::vector<adjacency_list<vecS, vecS, undirectedS, v, e>::vertex_descriptor> vd;
    std::vector<adjacency_list<vecS, vecS, undirectedS, v, e>::edge_descriptor> ed;
    adjacency_list<vecS, vecS, undirectedS, v, e>::vertex_descriptor startVertex;
    adjacency_list<vecS, vecS, undirectedS, v, e>::vertex_descriptor finishVertex;
    int mazes = 0;
    int levels = 0;
    int rows = 0;
    int columns = 0;
    int cells = 0;
    std::string start = "";
    std::string finish = "";
    
    std::ifstream input;
    input.open("input.txt");
    std::ofstream output;
    output.open("output.txt");
    
    input >> mazes;
    for(int m = 0; m < mazes; m++)
    {
        levels = 0;
        rows = 0;
        columns = 0;
        start = "";
        finish = "";
        input >> levels >> rows >> columns;
        cells = levels*rows*columns;

        std::cout << "Maze # " << m+1 << "\n\n";
    
        //Get start point and goal
        for(int i = 0; i < 6; i++)
        { 
            if(i < 3)
            {
                int temp = 0;
                input >> temp;
                start += std::to_string(temp);
            }
            else
            {
                int temp = 0;
                input >> temp;
                finish += std::to_string(temp);
            }
        }
    
        //Create vertices from input file
        for(int i = 0; i < levels; i++)
        {
            for(int j = 0; j < rows; j++)
            {
                for(int k = 0; k < columns; k++)
                {
                    v vertex;
                
                    input >> vertex.binaryCode;
                    vertex.location += std::to_string(i);
                    vertex.location += std::to_string(j);
                    vertex.location += std::to_string(k);
                    vertex.north = vertex.binaryCode[0];
                    vertex.east = vertex.binaryCode[1];
                    vertex.south = vertex.binaryCode[2];
                    vertex.west = vertex.binaryCode[3];
                    vertex.up = vertex.binaryCode[4];
                    vertex.down = vertex.binaryCode[5];
                
                    auto v = add_vertex(vertex, g);
                    vd.push_back(v);
                }
            }
        }

        //Create edges, give labels and add to graph
        for(int i = 0; i < cells; i++)
        {
            if(g[i].north == '1')
            {
                auto edge = add_edge(vd[i], vd[i-columns], g).first;
                g[edge].direction = 'N';
                ed.push_back(edge);
            }
            if(g[i].east == '1')
            {
                auto edge = add_edge(vd[i], vd[i+1], g).first;
                g[edge].direction = 'E';
                ed.push_back(edge);
            }
            if(g[i].south == '1')
            {
                auto edge = add_edge(vd[i], vd[i+columns], g).first;
                g[edge].direction = 'S';
                ed.push_back(edge);
            }
            if(g[i].west == '1')
            {
                auto edge= add_edge(vd[i], vd[i-1], g).first;
                g[edge].direction = 'W';
                ed.push_back(edge);
            }
            if(g[i].up == '1')
            {
                auto edge = add_edge(vd[i], vd[i+(columns*rows)], g).first;
                g[edge].direction = 'U';
                ed.push_back(edge);
            }
            if(g[i].down == '1')
            {
                auto edge = add_edge(vd[i], vd[i-(columns*rows)], g).first;
                g[edge].direction = 'D';
                ed.push_back(edge);
            }
        }
        

        std::vector<default_color_type> vertex_color(num_vertices(g));
        std::map<adjacency_list<vecS, vecS, undirectedS, v, e>::edge_descriptor, default_color_type> edge_color;
        auto idMap = get(vertex_index, g);
        auto vcMap = make_iterator_property_map(vertex_color.begin(), idMap);
        auto ecMap = make_assoc_property_map(edge_color);

        //Find vertices that matches start and finish and set them to starting point and goal
        for(int i = 0; i < vd.size(); i++)
        {
            if(g[i].location == start)
            {
                startVertex = vd[i];
            }
            if(g[i].location == finish)
            {
                finishVertex = vd[i];
            }
        }
    
        //Create visitor function for undirected_dfs()
        getPath vis;
        std::cout << "Starting Vertex: " << startVertex << "\n" << "Target Vertex: " <<finishVertex << "\n\n";
        undirected_dfs(g, vis, vcMap, ecMap, startVertex);

        //Iterate through list of path edges and remove all dead ends from path
        for(int i = 0; i < path.size(); i++)
        {
            if((target(path[i],g) != source(path[i+1],g)) && (target(path[i],g) != finishVertex))
            {
                path.erase(path.begin() + i);
                i = 0;
            }
        }

        for(int i = 0; i < path.size(); i++)
        {
            for(int j = 0; j < ed.size(); j++)
            {
                if((source(path[i], g) == source(ed[j], g)) && (target(path[i], g) == target(ed[j], g)))
                {
                    output << g[ed[j]].direction << ' ';
                }
                if(target(path[i], g) == finishVertex)
                {
                    output << g[path[i]].direction;
                    break;
                }

            }
            if(target(path[i], g) == finishVertex)
            {
                break;
            }
        }

        std::cout << '\n';

        for(auto p : path)
        {
            std::cout << p << '\n';
            if(target(p,g) == finishVertex)
                break;
        }
        std::cout << '\n';
        output << "\n\n";
        g.clear();
        path.clear();
        vd.clear();
        ed.clear();
        vertex_color.clear();
        edge_color.clear();
    }
    input.close();
    output.close();
}
