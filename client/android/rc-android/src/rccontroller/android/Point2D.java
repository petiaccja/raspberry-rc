////////////////////////////////////////////////////////////////////////////////
//	File: Point2D.java
//	Author: Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	A miniature class that represents points or vectors in 2D space.
//
////////////////////////////////////////////////////////////////////////////////

package rccontroller.android;

public class Point2D {
	Point2D() {
		x=y=0.f;
	}
	Point2D(Point2D other) {
		this.x = other.x;
		this.y = other.y;
	}
	Point2D(float x, float y) {
		this.x = x;
		this.y = y;
	}
	
	// coordinates
	public float x;
	public float y;
	
	// operation on this
	public Point2D add(Point2D p2) {
		x+=p2.x;
		y+=p2.y;
		return this;
	}
	public Point2D subtract(Point2D p2) {
		x-=p2.x;
		y-=p2.y;
		return this;
	}
	public float dot(Point2D p2) {
		return x*p2.x + y*p2.y;
	}
	public Point2D multiply(float c) {
		x*=c;
		y*=c;
		return this;
	}
	public Point2D divide(float c) {
		return multiply(1.f/c);
	}
	public Point2D multiply(Point2D p2) {
		x*=p2.x;
		y*=p2.y;
		return this;
	}
	public Point2D divide(Point2D p2) {
		x/=p2.x;
		y/=p2.y;
		return this;
	}
	
	// static operators
	public static Point2D add(Point2D p1, Point2D p2) {
		Point2D ret=null;
		try {ret = (Point2D)p1.clone();} catch (CloneNotSupportedException e) {};
		return ret.add(p2);
	}
	public static Point2D subtract(Point2D p1, Point2D p2) {
		Point2D ret=null;
		try {ret = (Point2D)p1.clone();} catch (CloneNotSupportedException e) {};
		return ret.subtract(p2);
	}
	public static float dot(Point2D p1, Point2D p2) {
		return p1.dot(p2);
	}
	public static Point2D multiply(Point2D p1, Point2D p2) {
		Point2D ret=null;
		try {ret = (Point2D)p1.clone();} catch (CloneNotSupportedException e) {};
		return ret.multiply(p2);
	}
	public static Point2D divide(Point2D p1, Point2D p2) {
		Point2D ret=null;
		try {ret = (Point2D)p1.clone();} catch (CloneNotSupportedException e) {};
		return ret.divide(p2);
	}
	public static Point2D multiply(Point2D p1, float c) {
		Point2D ret=null;
		try {ret = (Point2D)p1.clone();} catch (CloneNotSupportedException e) {};
		return ret.multiply(c);
	}
	public static Point2D divide(Point2D p1, float c) {
		Point2D ret=null;
		try {ret = (Point2D)p1.clone();} catch (CloneNotSupportedException e) {};
		return ret.divide(c);
	}
}