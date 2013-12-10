import java.awt.BasicStroke;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.LayoutManager;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.util.ArrayList;

import javax.swing.JApplet;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.LookAndFeel;
import javax.swing.SwingWorker;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;


public class CopyOfEvasionView extends JApplet implements EvasionListener, ActionListener, KeyListener{
  Object lock = new Object();
  class GPanel extends JPanel {
    /**
     * 
     */
    private static final long serialVersionUID = 4626419896777520312L;
    Graphics2D g2d;
    public GPanel() {
      setPreferredSize(new Dimension(500, 500));
    }

    @Override
    public void paintComponent(Graphics g) {
      super.paintComponent(g);
      g2d = (Graphics2D)g;
      g2d.setBackground(Color.WHITE);
      g2d.clearRect(0, 0, 400, 400);
      g2d.setColor(Color.GREEN);
      g2d.setStroke(new BasicStroke(1f));
      synchronized (lock) {
        for (Line2D wall : walls) {
          g2d.draw(wall);
        }
      }
      g2d.setColor(Color.BLACK);
      g2d.draw(new Line2D.Float(0,400,400,400));
      g2d.draw(new Line2D.Float(0,0, 400, 0));
      g2d.draw(new Line2D.Float(0,0,0, 400));
      g2d.draw(new Line2D.Float(400, 0, 400, 400));
      g2d.setColor(Color.BLUE);
      g2d.setStroke(new BasicStroke(3f,BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
      g2d.draw(new Line2D.Float(preyPosition,preyPosition));
      g2d.setColor(Color.RED);
      g2d.setStroke(new BasicStroke(5f));
      g2d.draw(new Line2D.Float(hunterPosition,hunterPosition));
    }
  }
  final EvasionModel model;
  String name;
  long move_counter;
  long time_counter;
  final int N;
  final int W;
  int moves_to_next_wall;
  Point2D preyPosition;
  Point2D hunterPosition;
  HunterMoves hunterDirection;
  ArrayList<Line2D> walls;
  String displayString;
  JFrame frame;
  GPanel mainPanel;
  JTextArea gameDescription;
  JButton hunterButton;
  Graphics2D graphics;
  public String winner = "not set";
  public String winnerScore = "not set";
  // A Button to click
  JButton okButton;
  // A textField to get text input
  JTextField nameField;
  JTextField scoreField;

  public CopyOfEvasionView(EvasionModel model, int N, int W) throws ClassNotFoundException, InstantiationException, IllegalAccessException, UnsupportedLookAndFeelException {
    this.model = model;
    this.N = N;
    this.W = W;
    preyPosition = new Point(330,200);
    hunterPosition = new Point(0, 0);
    moves_to_next_wall = N;
    time_counter = 0;
    move_counter = 0;
    hunterDirection = HunterMoves.SE;
    walls = new ArrayList<Line2D>();

    UIManager.setLookAndFeel(UIManager.getCrossPlatformLookAndFeelClassName());

  }

  public void init() {
    try {
      UIManager.setLookAndFeel(UIManager.getCrossPlatformLookAndFeelClassName());
    } catch (ClassNotFoundException e) {
      // TODO Auto-generated catch block
      e.printStackTrace();
    } catch (InstantiationException e) {
      // TODO Auto-generated catch block
      e.printStackTrace();
    } catch (IllegalAccessException e) {
      // TODO Auto-generated catch block
      e.printStackTrace();
    } catch (UnsupportedLookAndFeelException e) {
      // TODO Auto-generated catch block
      e.printStackTrace();
    }
    okButton = new JButton("Submit Score");
    nameField = new JTextField("Enter name",100);
    scoreField = new JTextField("Enter score",100);

    nameField.setBounds(20,40,100,30);
    scoreField.setBounds(20,75,100,30);

    okButton.setBounds(20,110,100,30);
    frame = new JFrame("Evasion");
    gameDescription = new JTextArea();
    mainPanel = new GPanel();
    mainPanel.setSize(400, 400);
    mainPanel.setBackground(Color.WHITE);
    //    mainPanel.setBorder(BorderFactory.createLineBorder(Color.black, 1));
    mainPanel.setVisible(true);
    Container frameContainer = frame.getContentPane();
    frameContainer.add(mainPanel,BorderLayout.CENTER);
    frameContainer.add(gameDescription,BorderLayout.NORTH);
    frame.setSize(600, 600);
    frame.setMaximumSize(new Dimension(600, 600));
    //    frame.setLocation(200, 200);
    //    frame.setExtendedState(JFrame.);
    frame.setVisible(true);
    frame.setResizable(false);
    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    frame.setVisible(true);
    add(okButton);
    okButton.addActionListener(this); 
    add(nameField);
    add(scoreField);
    add(frame);
  }

  public void stop() {

  }

  public void paint(Graphics g) {

  }

  @Override
  public void hunter_moved(HunterMove move, final String message) {
    move_counter++;
    hunterPosition = move.position;
    if (move.madeWall) {
      synchronized (lock) {
        walls.add(new Line2D.Float(move.start, move.end));
      }

    }
    else if (move.deletedWall) {
      synchronized (lock) {
        walls.remove(move.wallNumber-1);
      }
    }
    hunterDirection = move.move;
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        frame.getContentPane().validate();frame.getContentPane().repaint();
        gameDescription.setText(displayString + move_counter + message);
        return 0;
      }
    }.execute();
  }

  @Override
  public void prey_moved(Point2D move) {
    preyPosition = move;
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        mainPanel.repaint();
        return 0;
      }
    }.execute();
  }

  @Override
  public void prey_caught(long no_of_moves) {
    gameDescription.setText(displayString + ++move_counter + "Hunter Won!!!");
  }

  @Override
  public void game_started(final String hunterName,final String preyName) {
    displayString = "Hunter " + hunterName + " vs Prey " + preyName +". Moves:";
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        mainPanel.repaint();
        frame.repaint();
        gameDescription.setText(displayString + move_counter);
        return 0;
      }
    }.execute();
  }

  @Override
  public void game_reset() {


  }

  @Override
  public void time_over(long no_of_moves, final char player) {
    move_counter = no_of_moves;
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        switch (player) {
        case 'h':
          gameDescription.setText(displayString + move_counter +"Hunter Timed Out!!!");
          break;

        case 'p':
          gameDescription.setText(displayString + move_counter+"Prey Timed Out!!!");
          break;

        default:
          break;
        }
        frame.repaint();
        return 0;
      }
    }.execute();
  }

  @Override
  public String getName() {
    return null;
  }

  @Override
  public void actionPerformed(ActionEvent e) {
    // TODO Auto-generated method stub

  }

  @Override
  public void keyTyped(KeyEvent e) {
//    char c = e.getKeyChar();
//    switch(c) {
//    case 'h':
//    case 'H':
//
//      break;
//    case 'v':
//    case 'V':
//
//      break;
//    }
  }

  @Override
  public void keyPressed(KeyEvent e) {
    char c = e.getKeyChar();
    PreyMoves pMove = PreyMoves.ZZ;
    HunterMove hMove = new HunterMove();
    hMove.move = hunterDirection;
    switch(c) {
    case 'h':
    case 'H':
      Line2D hWall = new Line2D.Float(0, (float)hunterPosition.getY(), 199, (float)hunterPosition.getY());
      {
        float top = 0;
        float bottom = 199;

        for (Line2D wall : walls) {
          if (hWall.intersectsLine(wall)) {
            Point2D intersect = getIntersectionPoint((Line2D.Float)hWall, (Line2D.Float)wall);
            float x = (float)intersect.getX();
            if (x < hunterPosition.getX()) {
              if (x > top) {
                top = x;
              }
            }
            else if (x > hunterPosition.getX()) {
              if (x < bottom) {
                bottom = x;
              }
            }
          }
        }
        hMove.start = new Point2D.Float(top, (float)hunterPosition.getY());
        hMove.start = new Point2D.Float(bottom, (float)hunterPosition.getY());
      }
      hMove.move = HunterMoves.valueOf(hunterDirection.toString() + 'w');
      break;
      
    case 'v':
    case 'V':
      Line2D vWall = new Line2D.Float((float)hunterPosition.getX(), 0, (float)hunterPosition.getX(), 199);
      {
        float top = 0;
        float bottom = 199;

        for (Line2D wall : walls) {
          if (vWall.intersectsLine(wall)) {
            Point2D intersect = getIntersectionPoint((Line2D.Float)vWall, (Line2D.Float)wall);
            float x = (float)intersect.getY();
            if (x < hunterPosition.getY()) {
              if (x > top) {
                top = x;
              }
            }
            else if (x > hunterPosition.getY()) {
              if (x < bottom) {
                bottom = x;
              }
            }
          }
        }
        hMove.start = new Point2D.Float((float)hunterPosition.getX(),top);
        hMove.start = new Point2D.Float((float)hunterPosition.getX(),bottom);
      }
      hMove.move = HunterMoves.valueOf(hunterDirection.toString() + 'w');
      break;
     
    case 'z':
    case 'Z':
      hMove.move = HunterMoves.valueOf(hunterDirection.toString() + "wx");
      hMove.wallNumber = 0;
      break;
      
    case 'x':
    case 'X':
      hMove.move = HunterMoves.valueOf(hunterDirection.toString() + "wx");
      hMove.wallNumber = walls.size() - 1;
      break;
      
    case KeyEvent.VK_UP:
      pMove = PreyMoves.NN;
      break;
    case KeyEvent.VK_DOWN:
      pMove = PreyMoves.SS;
      break;
    case KeyEvent.VK_LEFT:
      pMove = PreyMoves.WW;
      break;
    case KeyEvent.VK_RIGHT:
      pMove = PreyMoves.EE;
      break;
    case KeyEvent.VK_NUMPAD1:
      pMove = PreyMoves.SW;
      break;
    case KeyEvent.VK_NUMPAD3:
      pMove = PreyMoves.SE;
      break;
    case KeyEvent.VK_NUMPAD7:
      pMove = PreyMoves.NW;
      break;
    case KeyEvent.VK_NUMPAD9:
      pMove = PreyMoves.NE;
      break;
    }
    if (pMove != PreyMoves.ZZ) {
      final HunterMove move = hMove;
      new SwingWorker<Integer, Integer>() {
        @Override
        protected Integer doInBackground() throws Exception {
          model.hunterMove(move);
          return 0;
        }
      }.execute();
    }
    else {
      final PreyMoves move = pMove;
      new SwingWorker<Integer, Integer>() {
        @Override
        protected Integer doInBackground() throws Exception {
          model.preyMove(move);
          return 0;
        }
      }.execute();
    }
  }

  @Override
  public void keyReleased(KeyEvent e) {
    // TODO Auto-generated method stub

  }

  public Point2D.Float getIntersectionPoint(Line2D.Float line1, Line2D.Float line2) {
    if (! line1.intersectsLine(line2) ) return null;
    double px = line1.getX1(),
        py = line1.getY1(),
        rx = line1.getX2()-px,
        ry = line1.getY2()-py;
    double qx = line2.getX1(),
        qy = line2.getY1(),
        sx = line2.getX2()-qx,
        sy = line2.getY2()-qy;

    double det = sx*ry - sy*rx;
    if (det == 0) {
      return null;
    } else {
      double z = (sx*(qy-py)+sy*(px-qx))/det;
      if (z==0 ||  z==1) return null;  // intersection at end point!
      return new Point2D.Float(
          (float)(px+z*rx), (float)(py+z*ry));
    }
  } // end intersection line-line

}
