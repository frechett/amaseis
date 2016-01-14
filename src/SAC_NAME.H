// sac_name.h
// Alan Jones
// 1995.02.09
// Array of sac file names, stations code, and component
class SAC_NAME {
  protected:
    char code[8];     // Station code
    char comp;        // Component (Z, N, or E)
  public:
    SAC_NAME(char *buffer);
    const char *GetName() const {return name;}
    const char *GetCode() const {return code;}
    const char GetComp() const {return comp;}
    char name[16];    // File name
    int operator == ( const SAC_NAME& sac_name) const {
      return strcmp(name,sac_name.GetName()) ? 0 : 1;
    }
};
// Array of SAC names
typedef TIArrayAsVector<SAC_NAME> SAC_NAMEarray;
