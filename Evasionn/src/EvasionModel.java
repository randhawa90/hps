import java.awt.Point;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.io.IOException;
import java.util.ArrayList;


public class EvasionModel {

	EvasionListener hunter;
	EvasionListener prey;
	EvasionListener view;
	final int boardSize = 50;
	final double epsilon = 10e-7;
	HunterMoves hunterDirection;
	Point2D hunterPosition;
	Point2D preyPosition;
	ArrayList<Line2D> walls;
	final int N;
	final int W;
	String hunterName;
	String preyName;
	volatile long moveUnitCounter;
	int nextWallTimeLeft;
	boolean hunterReady;
	public boolean isHunterReady() {
    return hunterReady;
  }

  public boolean isPreyReady() {
    return preyReady;
  }

  boolean preyReady;
	boolean gameStarted;
	long waitMillis;
	Object lock;

	public EvasionModel(int n, int w, long waitMillis) {
		N = n;
		W = w;
		this.waitMillis = waitMillis;
		nextWallTimeLeft = n;
		hunterPosition = new Point(0, 0);
		preyPosition = new Point(130, 100);
		hunterDirection = HunterMoves.SE;
		walls = new ArrayList<Line2D>();
		lock = new Object();
		preyReady = hunterReady = gameStarted = false;
	}

	public void sleepForSomeTime() {
		try {
			Thread.sleep(waitMillis);
		} catch (InterruptedException ignoreException) { }
	}

	public void hunterReady() throws IOException {
		synchronized (lock) {
			hunterReady = true;
//			if (preyReady) {
//				startGame();
//			}
		}
	}

	public void preyReady() throws IOException {
		synchronized (lock) {
			preyReady = true;
//			if (hunterReady) {
//				startGame();
//			}
		}
	}

	public void hunterMove(HunterMove move) throws IOException {
		incrMoveUnitCounter();
		sleepForSomeTime();
		makeHunterMove(move);
		if (nextWallTimeLeft > 0) {
			nextWallTimeLeft--;
		}
	}

	public void preyTimeOver() throws IOException {
		fireTimeOverEvent('p');
	}

	private void fireTimeOverEvent(char type) throws IOException {
		view.time_over(getMoveUnitCounter(), type);
		hunter.time_over(getMoveUnitCounter(), type);
		prey.time_over(getMoveUnitCounter(), type);
		switch (type) {
		case 'p':
			System.out.println("Prey Time over in "+getMoveUnitCounter()+" moves.");
			break;

		case 'h':
			System.out.println("Hunter Time over in "+getMoveUnitCounter()+" moves.");
			break;

		default:
			break;
		}
		//    logWriter.time_over(moveUnitCounter, type);
	}

	public void hunterTimeOver() throws IOException {
		fireTimeOverEvent('h');
	}

	private void firePreyCaughtEvent() throws IOException {
		view.prey_caught(getMoveUnitCounter());
		hunter.prey_caught(getMoveUnitCounter());
		prey.prey_caught(getMoveUnitCounter());
		System.out.println("Prey Caught in "+getMoveUnitCounter()+" moves.");
		//    logWriter.prey_caught(moveUnitCounter);
	}

	public void preyMove(PreyMoves move) throws IOException {
		sleepForSomeTime();
		preyPosition = makePreyMove(move);
		if (hasWon()) {
			firePreyCaughtEvent();
		}
		else {
			firePreyMovedEvent(preyPosition);
		}
	}

	private void firePreyMovedEvent(Point2D move) throws IOException {
		view.prey_moved(move);
		hunter.prey_moved(move);
		prey.prey_moved(move);
		//    logWriter.prey_moved(move);
	}

	public void register(EvasionListener el, char type) {
		switch (type) {
		case 'h':
			hunter = el;
			break;

		case 'p':
			prey = el;
			break;

		case 'v':
			view = el;
			break;

		case 'l':
			//        logWriter = el;
			break;

		default:
			break;
		}
	}

	public void startGame() throws IOException {
		if (gameStarted) {
			return;
		}
		else {
			gameStarted = true;
		}
		hunterName = hunter.getName();
		preyName = prey.getName();
		fireGameStartedEvent();
	}

	private void fireGameStartedEvent() throws IOException {
		view.game_started(hunterName, preyName);
		hunter.game_started(hunterName, preyName);
		prey.game_started(hunterName, preyName);
		//    logWriter.game_started(hunterName, preyName);
	}


	public synchronized void makeHunterMove(HunterMove move) throws IOException {
		HunterMove realMove = new HunterMove();
		realMove.move = hunterDirection;
		String message = "";
		switch (move.move) {
		case NEwx:
		case NWwx:
		case SEwx:
		case SWwx:
			if (move.wallNumber > 0 && walls.size() >= move.wallNumber) {
				walls.remove(move.wallNumber-1);
				realMove.deletedWall = true;
				realMove.wallNumber = move.wallNumber;
			}
			else {
				realMove.deletedWall = false;
				message = "Tried deleting Wall "+ move.wallNumber+" which does not exist";
			}
			break;
		default:
			break;
		}
		Point2D prevPos = new Point((Point) hunterPosition);
		hunterPosition = updateHunterPosAndDir();
		realMove.move = hunterDirection;
		realMove.position = hunterPosition;
		switch (move.move) {
		case NE:
		case NW:
		case SE:
		case SW:
			realMove.madeWall = false;
			realMove.deletedWall = false;
			break;
		case NEw:
		case NWw:
		case SEw:
		case SWw:
			realMove.madeWall = true;
			if (nextWallTimeLeft != 0) {
				realMove.madeWall = false;
			}
			if (walls.size() == W) {
				realMove.madeWall = false;
				break;
			}
			Line2D wallReq = new Line2D.Float(move.start,move.end);
			if (Math.abs(move.start.getX() - move.end.getX()) > epsilon && Math.abs(move.start.getY() - move.end.getY())> epsilon) {
				realMove.madeWall = false;
				message = "Wall not built as it was Diagonal";
			}
			else if (wallReq.ptSegDist(hunterPosition) <= epsilon) {
				realMove.madeWall = false;
				message = "Wall not built to prevent squishing";
			}
			else if (wallReq.ptSegDist(preyPosition) <= epsilon) {
				realMove.madeWall = false;
				message = "Wall not built as it passes through prey";
			}
			else if(wallReq.ptSegDist(prevPos) > epsilon) {
				realMove.madeWall = false;
				message = "Wall not built as it was not at hunter position";
			}
			else{
				boolean outOfBounds = false;
				double[] coords = new double[4];
				coords[0] = move.start.getX();
				coords[1] = move.start.getY();
				coords[2] = move.end.getX();
				coords[3] = move.end.getY();
				for (double d : coords) {
					if (0 - d > epsilon || d - 499 > epsilon) {
						outOfBounds = true;
						break;
					}
				}
				if (outOfBounds) {
					realMove.madeWall = false;
					message = "Wall not built as it was out of bounds";
				}
				else {
					for (Line2D wall : walls) {
						if (wall.intersectsLine(wallReq)) {
							realMove.madeWall = false;
							message = "Wall not built as it itersects with other walls";
							break;
						}
					}
				}
			}
			if (realMove.madeWall) {
				walls.add(wallReq);
				realMove.start = move.start;
				realMove.end = move.end;
				nextWallTimeLeft = N;
			}

			break;
		default:
			break;

		}
		if (hasWon()) {
			firePreyCaughtEvent();
		}
		else {
			fireHunterMovedEvent(realMove, message);
		}
	}

	private Point2D updateHunterPosAndDir() {
		try {
			walls.add(new Line2D.Float(-1,-1,500,-1));
			walls.add(new Line2D.Float(-1,-1,-1,500));
			walls.add(new Line2D.Float(-1,500,500,500));
			walls.add(new Line2D.Float(500,-1,500,500));
			Point2D movedTo;
			switch (hunterDirection) {
			case NE:
			case NEw:
			case NEwx:
				movedTo = new Point((int)hunterPosition.getX() + 1, (int)hunterPosition.getY() - 1);
				for (Line2D wall : walls) {
					if (wall.ptSegDist(movedTo) <= epsilon) {
						Point2D north = new Point((int)movedTo.getX(), (int)movedTo.getY() - 1);
						Point2D south = new Point((int)movedTo.getX(), (int)movedTo.getY() + 1);
						Point2D east = new Point((int)movedTo.getX() + 1, (int)movedTo.getY());
						Point2D west = new Point((int)movedTo.getX() - 1, (int)movedTo.getY());
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(south) <= epsilon) {
								for (Line2D adjWall : walls) {
									if (adjWall.ptSegDist(west) <= epsilon) {
										hunterDirection = HunterMoves.SW;
										return hunterPosition;
									}
								}
								hunterDirection = HunterMoves.NW;
								return west;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(west) <= epsilon) {
								hunterDirection = HunterMoves.SE;
								return south;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(north) <= epsilon) {
								hunterDirection = HunterMoves.NW;
								return west;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(east) <= epsilon) {
								hunterDirection = HunterMoves.SE;
								return south;
							}
						}
						hunterDirection = HunterMoves.SE;
						return south;
					}
				}
				return movedTo;
			case NW:
			case NWw:
			case NWwx:
				movedTo = new Point((int)hunterPosition.getX() -1, (int)hunterPosition.getY() - 1);
				for (Line2D wall : walls) {
					if (wall.ptSegDist(movedTo) <= epsilon) {
						Point2D north = new Point((int)movedTo.getX(), (int)movedTo.getY() - 1);
						Point2D south = new Point((int)movedTo.getX(), (int)movedTo.getY() + 1);
						Point2D east = new Point((int)movedTo.getX() + 1, (int)movedTo.getY());
						Point2D west = new Point((int)movedTo.getX() - 1, (int)movedTo.getY());
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(south) <= epsilon) {
								for (Line2D adjWall : walls) {
									if (adjWall.ptSegDist(east) <= epsilon) {
										hunterDirection = HunterMoves.SE;
										return hunterPosition;
									}
								}
								hunterDirection = HunterMoves.NE;
								return east;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(west) <= epsilon) {
								hunterDirection = HunterMoves.SW;
								return south;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(north) <= epsilon) {
								hunterDirection = HunterMoves.NE;
								return east;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(east) <= epsilon) {
								hunterDirection = HunterMoves.SW;
								return south;
							}
						}
						hunterDirection = HunterMoves.SW;
						return south;
					}
				}
				return movedTo;
			case SE:
			case SEw:
			case SEwx:
				movedTo = new Point((int)hunterPosition.getX() + 1, (int)hunterPosition.getY() + 1);
				for (Line2D wall : walls) {
					if (wall.ptSegDist(movedTo) <= epsilon) {
						Point2D north = new Point((int)movedTo.getX(), (int)movedTo.getY() - 1);
						Point2D south = new Point((int)movedTo.getX(), (int)movedTo.getY() + 1);
						Point2D east = new Point((int)movedTo.getX() + 1, (int)movedTo.getY());
						Point2D west = new Point((int)movedTo.getX() - 1, (int)movedTo.getY());
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(north) <= epsilon) {
								for (Line2D adjWall : walls) {
									if (adjWall.ptSegDist(west) <= epsilon) {
										hunterDirection = HunterMoves.NW;
										return hunterPosition;
									}
								}
								hunterDirection = HunterMoves.SW;
								return west;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(west) <= epsilon) {
								hunterDirection = HunterMoves.NE;
								return north;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(south) <= epsilon) {
								hunterDirection = HunterMoves.SW;
								return west;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(east) <= epsilon) {
								hunterDirection = HunterMoves.NE;
								return north;
							}
						}
						hunterDirection = HunterMoves.NE;
						return north;
					}
				}
				return movedTo;

			case SW:
			case SWw:
			case SWwx:
				movedTo = new Point((int)hunterPosition.getX() - 1, (int)hunterPosition.getY() + 1);
				for (Line2D wall : walls) {
					if (wall.ptSegDist(movedTo) <= epsilon) {
						Point2D north = new Point((int)movedTo.getX(), (int)movedTo.getY() - 1);
						Point2D south = new Point((int)movedTo.getX(), (int)movedTo.getY() + 1);
						Point2D east = new Point((int)movedTo.getX() + 1, (int)movedTo.getY());
						Point2D west = new Point((int)movedTo.getX() - 1, (int)movedTo.getY());
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(north) <= epsilon) {
								for (Line2D adjWall : walls) {
									if (adjWall.ptSegDist(east) <= epsilon) {
										hunterDirection = HunterMoves.NE;
										return hunterPosition;
									}
								}
								hunterDirection = HunterMoves.SE;
								return east;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(west) <= epsilon) {
								hunterDirection = HunterMoves.NW;
								return north;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(south) <= epsilon) {
								hunterDirection = HunterMoves.SE;
								return east;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(east) <= epsilon) {
								hunterDirection = HunterMoves.NW;
								return north;
							}
						}
						hunterDirection = HunterMoves.NW;
						return north;
					}
				}
				return movedTo;

			default:
				return hunterPosition;
			}
		}
		finally {
			walls.remove(walls.size()-1);
			walls.remove(walls.size()-1);
			walls.remove(walls.size()-1);
			walls.remove(walls.size()-1);
		}
	}

	/**
	 * @return the moveUnitCounter
	 */
	public long getMoveUnitCounter() {
		synchronized (lock) {
			return moveUnitCounter;
		}
	}

	/**
	 * @param moveUnitCounter the moveUnitCounter to set
	 */
	public void setMoveUnitCounter(long moveUnitCounter) {
		synchronized (lock) {
			this.moveUnitCounter = moveUnitCounter;
		}
	}

	public void incrMoveUnitCounter() {
		synchronized (lock) {
			this.moveUnitCounter++;
		}
	}

	private void fireHunterMovedEvent(HunterMove move, String message) throws IOException {
		view.hunter_moved(move, message);
		hunter.hunter_moved(move, message);
		prey.hunter_moved(move, message);
		//    logWriter.hunter_moved(move, message);
	}

	public synchronized Point2D makePreyMove(PreyMoves move) {
		try {
			walls.add(new Line2D.Float(-1,-1,500,-1));
			walls.add(new Line2D.Float(-1,-1,-1,500));
			walls.add(new Line2D.Float(-1,500,500,500));
			walls.add(new Line2D.Float(500,-1,500,500));
			Point2D movedTo;
			switch (move) {
			case ZZ:
				return preyPosition;

			case NN:
				movedTo = new Point((int)preyPosition.getX(), (int)preyPosition.getY() - 1);
				for (Line2D wall : walls) {
					if (wall.ptSegDist(movedTo) <= epsilon) {
						return preyPosition;
					}
				}
				return movedTo;

			case SS:
				movedTo = new Point((int)preyPosition.getX(), (int)preyPosition.getY() + 1);
				for (Line2D wall : walls) {
					if (wall.ptSegDist(movedTo) <= epsilon) {
						return preyPosition;
					}
				}
				return movedTo;

			case EE:
				movedTo = new Point((int)preyPosition.getX() + 1, (int)preyPosition.getY());
				for (Line2D wall : walls) {
					if (wall.ptSegDist(movedTo) <= epsilon) {
						return preyPosition;
					}
				}
				return movedTo;

			case WW:
				movedTo = new Point((int)preyPosition.getX() - 1, (int)preyPosition.getY());
				for (Line2D wall : walls) {
					if (wall.ptSegDist(movedTo) <= epsilon) {
						return preyPosition;
					}
				}
				return movedTo;
				//TO-DO refine these
			case NE:
				movedTo = new Point((int)preyPosition.getX() + 1, (int)preyPosition.getY() - 1);
				for (Line2D wall : walls) {
					if (wall.ptSegDist(movedTo) <= epsilon) {
						Point2D north = new Point((int)movedTo.getX(), (int)movedTo.getY() - 1);
						Point2D south = new Point((int)movedTo.getX(), (int)movedTo.getY() + 1);
						Point2D east = new Point((int)movedTo.getX() + 1, (int)movedTo.getY());
						Point2D west = new Point((int)movedTo.getX() - 1, (int)movedTo.getY());
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(south) <= epsilon) {
								for (Line2D adjWall : walls) {
									if (adjWall.ptSegDist(west) <= epsilon) {
										return preyPosition;
									}
								}
								return west;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(west) <= epsilon) {
								return south;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(north) <= epsilon) {
								return west;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(east) <= epsilon) {
								return south;
							}
						}
						return south;
					}
				}
				return movedTo;


			case NW:
				movedTo = new Point((int)preyPosition.getX() -1, (int)preyPosition.getY() - 1);
				for (Line2D wall : walls) {
					if (wall.ptSegDist(movedTo) <= epsilon) {
						Point2D north = new Point((int)movedTo.getX(), (int)movedTo.getY() - 1);
						Point2D south = new Point((int)movedTo.getX(), (int)movedTo.getY() + 1);
						Point2D east = new Point((int)movedTo.getX() + 1, (int)movedTo.getY());
						Point2D west = new Point((int)movedTo.getX() - 1, (int)movedTo.getY());
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(south) <= epsilon) {
								for (Line2D adjWall : walls) {
									if (adjWall.ptSegDist(east) <= epsilon) {
										return preyPosition;
									}
								}
								return east;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(west) <= epsilon) {
								return south;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(north) <= epsilon) {
								return east;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(east) <= epsilon) {
								return south;
							}
						}
						return south;
					}
				}
				return movedTo;

			case SE:
				movedTo = new Point((int)preyPosition.getX() + 1, (int)preyPosition.getY() + 1);
				for (Line2D wall : walls) {
					if (wall.ptSegDist(movedTo) <= epsilon) {
						Point2D north = new Point((int)movedTo.getX(), (int)movedTo.getY() - 1);
						Point2D south = new Point((int)movedTo.getX(), (int)movedTo.getY() + 1);
						Point2D east = new Point((int)movedTo.getX() + 1, (int)movedTo.getY());
						Point2D west = new Point((int)movedTo.getX() - 1, (int)movedTo.getY());
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(north) <= epsilon) {
								for (Line2D adjWall : walls) {
									if (adjWall.ptSegDist(west) <= epsilon) {
										return preyPosition;
									}
								}
								return west;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(west) <= epsilon) {
								return north;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(south) <= epsilon) {
								return west;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(east) <= epsilon) {
								return north;
							}
						}
						return north;
					}
				}
				return movedTo;

			case SW:
				movedTo = new Point((int)preyPosition.getX() - 1, (int)preyPosition.getY() + 1);
				for (Line2D wall : walls) {
					if (wall.ptSegDist(movedTo) <= epsilon) {
						Point2D north = new Point((int)movedTo.getX(), (int)movedTo.getY() - 1);
						Point2D south = new Point((int)movedTo.getX(), (int)movedTo.getY() + 1);
						Point2D east = new Point((int)movedTo.getX() + 1, (int)movedTo.getY());
						Point2D west = new Point((int)movedTo.getX() - 1, (int)movedTo.getY());
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(north) <= epsilon) {
								for (Line2D adjWall : walls) {
									if (adjWall.ptSegDist(east) <= epsilon) {
										return preyPosition;
									}
								}
								return east;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(west) <= epsilon) {
								return north;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(south) <= epsilon) {
								return east;
							}
						}
						for (Line2D neighbourWall : walls) {
							if (neighbourWall.ptSegDist(east) <= epsilon) {
								return north;
							}
						}
						return north;
					}
				}
				return movedTo;
			default:
				return preyPosition;
			}
		}
		finally {
			walls.remove(walls.size()-1);
			walls.remove(walls.size()-1);
			walls.remove(walls.size()-1);
			walls.remove(walls.size()-1);
		}
	}

	public boolean hasWon() {
		Line2D distance = new Line2D.Float(hunterPosition, preyPosition);
		for (Line2D wall : walls) {
			if (distance.intersectsLine(wall)) {
				return false;
			}
		}
		return ((4.0000000 - hunterPosition.distance(preyPosition)) > epsilon);
	}
}
