interface DefineParams {
  name: string, 
  type: string, // int, bool, char, className
  kind: string  // field, static, arg, var
}

/* Defines a new variable, adds it to the table and increments the index for that kind */
define({ name, type, kind }: DefineParams): void { 
  const index: number = this.indexCounter[kind];
  this.table[name] = { type, kind, index };
  this.indexCounter[kind] += 1;
}

    /* Defines a new variable, adds it to the table and increments the index for that kind */
    define({ name, type, kind }) {
      const index = this.indexCounter[kind];
      this.table[name] = { type, kind, index };
      this.indexCounter[kind] += 1;
  }