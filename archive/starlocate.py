


class Sight():
    def __init__(self):
        """
        Class storing all infrmation on  a sight inclusing time, angle, reference body, GP, distance to GP.
        input array(Star name, sight angle)
        """
        
        return


def triangulate(args):
    """
    Uses all current sights to estimate the position on earth in longditude and latitude.

    Input: array of active Sights.
    Outut: Long, Lat pure numerical form.
    """
    return

def add_sight():
    """
    Callable from cmd. 

    Adds a sight to the list of active sights.
    
    Input: Star name and sight angle
    """
    return

def list_sight():
    """
    Prints list of sights in pretty format.
    """
    return

def rm_sight():
    """
    Deletes a specific sight from the lsit of active Sights. No arguments means last entry is deleted.
    Confirms with user since this operation cannot be undone.
    """
    return

def ra_dec_2_vec():
    """
    Converts Right Ascension and Declination into a vector on unit spehere.
    Input: array[RA, DEC]
    Outut: array[x,y,z]
    """
    return

def vec_2_ra_dec():
    """
    Converts a vector to agular coodiantes.
    Input: array[x,y,z]
    Output: array[RA, DEC]
    """
    return

def timestamp_2_sidereal():
    """
    Converts pc clock to sidereal time usig approximation.
    """
    return

def J2000_2_current_ra_dec():
    """
    takes J2000 epoch coordinates and appromxiates the position of the star accoding to current time.
    Input: array[RA,DEC]
    """
    return

def locate():
    """
    Runs the nessesary steps to find a location according to Sights.
    Run triangulation script with list of active sights. 
    Warns user if there are very outdated sights or any sights potnetially skweing results.
    Prettifies the long lat numbers. 
    Prints.
    """
    return

def lon_lat_format():
    """
    Makes lon and lat numbers well formed.
    Input: array(lon, lat)
    Output: String of lon lat with NESW notation.
    """
    return

def deg_min_sec_2_deg():
    """
    Handles various angle formats and converts to degrees.
    Input: String
    Output: Float
    """
    return

