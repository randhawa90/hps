import java.awt.Point;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;


public class PlayerServer {

  final EvasionModel model;
  String name;
  int move_counter;
  long time_counter;
  final long totalTime;
  final int N;
  final int W;
  int moves_to_next_wall;
  Point2D preyPosition;
  Point2D hunterPosition;
  HunterMoves hunterDirection;
  ArrayList<Line2D> walls;
  final int portNumber;
  PrintWriter out;
  BufferedReader in;
  ServerSocket serverSocket;
  Socket clientSocket;
  
  public PlayerServer(EvasionModel model, int N, int W, long totalTime, int portNumber) {
    this.model = model;
    this.N = N;
    this.W = W;
    this.totalTime = totalTime;
    preyPosition = new Point(330,200);
    hunterPosition = new Point(0, 0);
    moves_to_next_wall = N;
    time_counter = 0;
    move_counter = 0;
    this.portNumber = portNumber;
    hunterDirection = HunterMoves.SE;
    walls = new ArrayList<Line2D>();
  }
  
  public void init_connection() throws IOException {
    serverSocket = new ServerSocket(portNumber);
    clientSocket = serverSocket.accept();
    out = new PrintWriter(clientSocket.getOutputStream(), true);
    in = new BufferedReader( new InputStreamReader(clientSocket.getInputStream()));
    out.println("Team Name?");
    while(!in.ready());
    name = in.readLine();
    out.println(N +" "+W);
  }
  
  public String get_move() throws IOException {
    String move;
    in = new BufferedReader( new InputStreamReader(clientSocket.getInputStream()));
    boolean timeOver = false;
    time_counter -= 40000000;
    out.println(((totalTime - time_counter)/1000000000.0));
    long time = System.nanoTime();
    while (!in.ready()) {
      long time2 = System.nanoTime();
      time_counter += time2 - time;
      time = time2;
      if (time_counter > totalTime) {
        timeOver = true;
        break;
      }
    }
    if (timeOver) {
      move = "T";
    }
    else {
      while (!in.ready());
      move = in.readLine();
    }
    return move;
  }
  
  public void send_details() {
    out.println("Walls");
    out.println(walls.size());
    int seq = 1;
    for (Line2D wall : walls) {
      out.println(seq++ + " ("+(int)wall.getX1()+","+(int)wall.getY1()+"),("+(int)wall.getX2()+","+(int)wall.getY2()+")");
    }
    out.println("Moves to Next Wall Build");
    out.println(moves_to_next_wall);
    out.println("H"+" "+hunterDirection+" "+"("+(int)hunterPosition.getX()+","+(int)hunterPosition.getY()+")");
    out.println("P ("+(int)preyPosition.getX()+","+(int)preyPosition.getY()+")");
  }
  
  public void close_conn_game_over(String message) throws IOException {
    out.println(message);
    out.println(move_counter);
    out.close();
    in.close();
    clientSocket.close();
    serverSocket.close();
  }
}
