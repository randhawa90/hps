import java.awt.Point;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.io.IOException;
import java.util.ArrayList;

import javax.swing.SwingWorker;


public class Hunter implements EvasionListener {
  final EvasionModel model;
  String name;
  int move_counter;
  long time_counter;
  final int N;
  final int W;
  int moves_to_next_wall;
  Point2D preyPosition;
  Point2D hunterPosition;
  HunterMoves hunterDirection;
  ArrayList<Line2D> walls;
  public Hunter(EvasionModel model, int N, int W) {
//    super(model, N, W, totalTime, portNumber);
    this.model = model;
    this.N = N;
    this.W = W;
    preyPosition = new Point(130,100);
    hunterPosition = new Point(0, 0);
    moves_to_next_wall = N;
    move_counter = 0;
    hunterDirection = HunterMoves.SE;
    walls = new ArrayList<Line2D>();
  }

  public void make_move(HunterMove move,long wait) throws IOException {
    long time = System.currentTimeMillis();
    while(System.currentTimeMillis() - time < wait);
    model.hunterMove(move);
  }

  public void make_move() throws IOException {
    HunterMove move = new HunterMove();
    model.hunterMove(move);
  }
  
  @Override
  public void hunter_moved(HunterMove move, String message) throws IOException {
    move_counter++;
    if (moves_to_next_wall > 0) {
      moves_to_next_wall--;
    }
    hunterPosition = move.position;
    if (move.madeWall) {
      walls.add(new Line2D.Float(move.start, move.end));
      moves_to_next_wall = N;
    }
    else if (move.deletedWall) {
      walls.remove(move.wallNumber-1);
    }
    hunterDirection = move.move;
//    if ((move_counter%2) == 1) {
//      new SwingWorker<Integer, Integer>() {
//        @Override
//        protected Integer doInBackground() throws Exception {
//          make_move();
//          return 0;
//        }
//      }.execute();
//    }
  }

  @Override
  public void prey_moved(Point2D move) throws IOException {
    preyPosition = move;
//    new SwingWorker<Integer, Integer>() {
//      @Override
//      protected Integer doInBackground() throws Exception {
//        make_move();
//        return 0;
//      }
//    }.execute();
  }

  @Override
  public void prey_caught(long no_of_moves) {
//    new SwingWorker<Integer, Integer>() {
//      @Override
//      protected Integer doInBackground() throws Exception {
//        close_conn_game_over("You Won!!!");
//        return 0;
//      }
//    }.execute();
  }

  @Override
  public void game_started(String hunterName, String preyName) {
//    new SwingWorker<Integer, Integer>() {
//      @Override
//      protected Integer doInBackground() throws Exception {
//        make_move();
//        return 0;
//      }
//    }.execute();
  }

  @Override
  public void game_reset() {

  }

  @Override
  public void time_over(long no_of_moves, final char player) {
    
  }

  @Override
  public String getName() {
    return name;
  }

}
