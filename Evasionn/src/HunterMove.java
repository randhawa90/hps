import java.awt.geom.Point2D;

public class HunterMove {
  public HunterMoves move;
  public Point2D position;
  public Point2D start = null;
  public Point2D end = null;
  public int wallNumber = -1;
  boolean madeWall = false;
  boolean deletedWall = false;
  @Override
  public String toString() {
    String str = "";
    str+=move.toString();
    if (madeWall) {
      str += "("+(int)start.getX()+","+(int)start.getY()+"),("+(int)end.getX()
          +","+(int)end.getY()+")";
    }
    else if(deletedWall) {
      str += wallNumber;
    }
    return str;
  }
}
