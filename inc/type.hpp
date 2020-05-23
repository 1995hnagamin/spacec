#ifndef TYPE_HPP
#define TYPE_HPP

class Type {
  public:
    enum class TK {
      IntN,
    };

  public:
    Type(TK k): kind(k) {
    }
    virtual ~Type() = 0;
    TK get_kind() const {
      return kind;
    }

  private:
    TK const kind;
};

class IntNType : public Type {
  public:
    IntNType(int w): Type(TK::IntN), width(w) {
    }
    int get_width() const {
      return width;
    }

  private:
    int const width;
};

#endif /* !TYPE_HPP */
