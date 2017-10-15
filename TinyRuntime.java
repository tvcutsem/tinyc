import java.util.HashMap;
import java.util.Map;
import java.lang.reflect.Array;

/**
 * TinyRuntime provides support for multi-dimensional arrays.
 * These methods are invoked to handle the dimensions of the arrays.
 */

public class TinyRuntime {

  private static Map __ARRAYDIMENSIONS__ = new HashMap();

  /**
   * Called when a multidimensional array is instantiated.
   * When Tiny instantiates a multidimensional array, it needs to keep
   * track of the individual dimension lengths (to be able to calculate
   * offsets). This is needed because all arrays in the runtime level
   * are actually ONE-dimensional arrays.
   */
  public static void declareArray(Object array, int[] dimensions) {
    __ARRAYDIMENSIONS__.put(array,dimensions);
  }

  /**
   * Method for retrieving the dimension length of a multi-dimensional
   * array. This is needed when calculating offsets for multi-dimensional
   * array access.
   */
  public static int dimension(Object array, int dim) throws Exception {
    int[] dimensions = (int[]) __ARRAYDIMENSIONS__.get(array);
    if (dimensions == null)
       throw new Exception(
         "Runtime error: multidimensional array not properly declared: "+
         "arraysize = "+Array.getLength(array)+" dimension = "+dim);
    return dimensions[dim];
  }

}
